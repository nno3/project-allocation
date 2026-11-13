#include "allocation.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// reading staff files
void readStaff(const string &filename, StaffMap &staffMap)
{
    ifstream in(filename);
    if (!in)
    {
        cerr << "Cannot open staff file: " << filename << "\n";
        exit(1);
    }

    string line;
    while (getline(in, line))
    {
        if (line.empty())
            continue;
        istringstream iss(line);

        Staff s;
        iss >> s.id >> s.load;

        string subj;
        while (iss >> subj)
            s.subjects.push_back(subj);

        staffMap[s.id] = s;
    }
}

// reading project files
void readProjects(const string &filename, ProjectMap &projectMap)
{
    ifstream in(filename);
    if (!in)
    {
        cerr << "Cannot open projects file: " << filename << "\n";
        exit(1);
    }

    string line;
    while (getline(in, line))
    {
        if (line.empty())
            continue;
        istringstream iss(line);

        Project p;
        iss >> p.id >> p.proposer >> p.multiplicity >> p.subject;

        getline(iss, p.title);
        if (!p.title.empty() && p.title[0] == ' ')
            p.title.erase(0, 1);

        projectMap[p.id] = p;
    }
}

// read student files
void readStudents(const string &filename, StudentMap &studentMap, vector<string> &studentOrder)
{
    ifstream in(filename.c_str());
    if (!in)
    {
        cerr << "Cannot open students file: " << filename << "\n";
        exit(1);
    }

    string line;
    while (getline(in, line))
    {
        if (line.empty())
            continue;
        istringstream iss(line);

        Student s;
        iss >> s.id;

        int pid;
        while (iss >> pid)
            s.choices.push_back(pid);

        studentMap[s.id] = s;
        studentOrder.push_back(s.id);
    }

    sort(studentOrder.begin(), studentOrder.end());
}

// helpers with utility functions
bool staffHasSubject(const Staff &staff, const string &subject)
{
    return find(staff.subjects.begin(),
                staff.subjects.end(),
                subject) != staff.subjects.end();
}

int studentPreferenceScore(const Student &stu, int projectId)
{
    for (size_t i = 0; i < stu.choices.size(); ++i)
        if (stu.choices[i] == projectId)
            return 4 - (int)i;
    return 0;
}

int supervisorPreferenceScore(const Staff &staff, const Project &project)
{
    if (project.proposer == staff.id)
        return 4;
    if (staffHasSubject(staff, project.subject))
        return 2;
    return 0;
}

// phase 1
// preference-round based allocation no supervisors yet
// A student is assigned the highest-ranked project that still has free multiplicity.
void phase1(const StudentMap &students, const vector<string> &studentOrder, const ProjectMap &projects, AllocMap &alloc, map<int, int> &projectCount)
{
    // initialise allocations
    for (const auto &sid : studentOrder)
        alloc[sid] = Allocation();

    // it starts the round by allocating first choices then Second choices then third choices and fourth choices
    for (int pref = 0; pref < 4; ++pref)
    {
        for (const string &sid : studentOrder)
        {
            Allocation &a = alloc[sid];
            if (a.projectId != -1)
                continue;

            const Student &stu = students.at(sid);
            if (pref >= (int)stu.choices.size())
                continue;

            int pid = stu.choices[pref];
            auto pit = projects.find(pid);
            if (pit == projects.end())
                continue;

            if (projectCount[pid] < pit->second.multiplicity)
            {
                a.projectId = pid;
                projectCount[pid]++;
            }
        }
    }
}

// phase 2 helpers
// assigns a supervisor to students who are already allocated to a specific project from phase 1, but do not yet have a supervisor assigned, stops when staff is full and doesn't assign new projects
static void assignExistingStudentsToStaff(const string &staffId, int projectId, const StaffMap &staffMap, AllocMap &alloc, map<string, int> &staffCount)
{
    int load = staffMap.at(staffId).load;

    for (auto &entry : alloc)
    {
        if (staffCount[staffId] >= load)
            break;

        Allocation &a = entry.second;
        if (a.projectId == projectId && a.supervisorId.empty())
        {
            a.supervisorId = staffId;
            staffCount[staffId]++;
        }
    }
}

// allows a supervisor to take on students who have no project assigned yet, and put them onto the given project. It make sure staff load is not exceeded and project multiplicity is not exceeded
static void assignUnallocatedStudentsToProject(const string &staffId, int projectId, const StaffMap &staffMap, const ProjectMap &projects, const vector<string> &studentOrder, AllocMap &alloc, map<int, int> &projectCount, map<string, int> &staffCount)
{
    int load = staffMap.at(staffId).load;
    int cap = projects.at(projectId).multiplicity;

    for (const string &sid : studentOrder)
    {
        if (staffCount[staffId] >= load)
            break;

        Allocation &a = alloc[sid];
        if (a.projectId != -1)
            continue;

        if (projectCount[projectId] >= cap)
            break;

        a.projectId = projectId;
        a.supervisorId = staffId;
        projectCount[projectId]++;
        staffCount[staffId]++;
    }
}

// phase 2.1
// assign staff to supervise students who already have their projects from phase 1.
// if they still have load left, assign unallocated students to their projects
void phase2_1(const StaffMap &staffMap, const ProjectMap &projects, const StudentMap &, const vector<string> &studentOrder, AllocMap &alloc, map<int, int> &projectCount, map<string, int> &staffCount)
{
    for (const auto &s : staffMap)
    {
        const string &staffId = s.first;

        for (const auto &p : projects)
        {
            if (p.second.proposer != staffId)
                continue;

            assignExistingStudentsToStaff(staffId, p.first, staffMap, alloc, staffCount);

            if (projectCount[p.first] < p.second.multiplicity)
                assignUnallocatedStudentsToProject(staffId, p.first, staffMap, projects, studentOrder, alloc, projectCount, staffCount);
        }
    }
}

// phase 2.2
// after proposers are satisfied, supervisors can take additional students from projects within their areas of expertise.
void phase2_2(const StaffMap &staffMap, const ProjectMap &projects, const StudentMap &, const vector<string> &studentOrder, AllocMap &alloc, map<int, int> &projectCount, map<string, int> &staffCount)
{
    for (const auto &s : staffMap)
    {
        const string &staffId = s.first;
        const Staff &staff = s.second;

        for (const auto &p : projects)
        {
            if (p.second.proposer == staffId)
                continue;
            if (!staffHasSubject(staff, p.second.subject))
                continue;

            assignExistingStudentsToStaff(staffId, p.first, staffMap, alloc, staffCount);

            if (projectCount[p.first] < p.second.multiplicity)
                assignUnallocatedStudentsToProject(staffId, p.first, staffMap, projects, studentOrder, alloc, projectCount, staffCount);
        }
    }
}

// phase 2.3
// If supervisors still have unused load, they may supervise any project that still has multiplicity. This ensures the algorithm always terminates with a complete allocation.
void phase2_3(const StaffMap &staffMap, const ProjectMap &projects, const StudentMap &, const vector<string> &studentOrder, AllocMap &alloc, map<int, int> &projectCount, map<string, int> &staffCount)
{
    for (const auto &s : staffMap)
    {
        const string &staffId = s.first;

        for (const auto &p : projects)
        {
            assignExistingStudentsToStaff(staffId, p.first, staffMap, alloc, staffCount);

            if (projectCount[p.first] < p.second.multiplicity)
                assignUnallocatedStudentsToProject(staffId, p.first, staffMap, projects, studentOrder, alloc, projectCount, staffCount);
        }
    }
}

// score
int computeTotalScore(const StaffMap &staffMap, const ProjectMap &projects, const StudentMap &students, const AllocMap &alloc)
{
    int total = 0;

    for (const auto &e : alloc)
    {
        const Allocation &a = e.second;
        total += studentPreferenceScore(students.at(e.first), a.projectId);
        total += supervisorPreferenceScore(staffMap.at(a.supervisorId), projects.at(a.projectId));
    }
    return total;
}
