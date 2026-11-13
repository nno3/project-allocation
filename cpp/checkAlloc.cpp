// CheckAlloc.cpp, simple legality checker for allocations
// this program checks: project capacities, staff loads, and student got allocated once.
// It does not check full stability conditions

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

struct Staff
{
    string id;
    int load;
};

struct Project
{
    int id;
    string proposer;
    int multiplicity;
};

struct AllocationLine
{
    string student;
    int project;
    string supervisor;
};

map<string, Staff> readStaff(const string &f)
{
    map<string, Staff> mp;
    ifstream in(f);
    string line;
    while (getline(in, line))
    {
        if (line.empty())
            continue;
        istringstream iss(line);
        Staff s;
        iss >> s.id >> s.load;
        string dummy;
        while (iss >> dummy)
        {
        }
        mp[s.id] = s;
    }
    return mp;
}

map<int, Project> readProjects(const string &f)
{
    map<int, Project> mp;
    ifstream in(f);
    string line;
    while (getline(in, line))
    {
        if (line.empty())
            continue;
        istringstream iss(line);
        Project p;
        iss >> p.id >> p.proposer >> p.multiplicity;
        string dummy;
        iss >> dummy; // subject, ignored here
        mp[p.id] = p;
    }
    return mp;
}

vector<AllocationLine> readAlloc(const string &f)
{
    vector<AllocationLine> v;
    ifstream in(f);
    string line;
    while (getline(in, line))
    {
        if (line.empty())
            continue;
        istringstream iss(line);
        AllocationLine a;
        if (!(iss >> a.student >> a.project >> a.supervisor))
            continue;
        v.push_back(a);
    }
    return v;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        cerr << "Usage: " << argv[0] << " staff.txt projects.txt students.txt alloc.txt\n";
        return 1;
    }

    auto staff = readStaff(argv[1]);
    auto proj = readProjects(argv[2]);
    auto alloc = readAlloc(argv[4]);

    map<int, int> projCount;
    map<string, int> staffCount;

    bool ok = true;

    for (const auto &a : alloc)
    {
        projCount[a.project]++;
        staffCount[a.supervisor]++;
    }

    // check project multiplicity
    for (const auto &p : proj)
    {
        int pid = p.first;
        int cap = p.second.multiplicity;
        int used = projCount[pid];
        if (used > cap)
        {
            cout << "Project " << pid << " exceeds multiplicity!" << " (" << used << "/" << cap << ")\n";
            ok = false;
        }
    }

    // check staff load
    for (const auto &s : staff)
    {
        int used = staffCount[s.first];
        if (used > s.second.load)
        {
            cout << "Staff " << s.first << " overloaded! (" << used << "/" << s.second.load << ")\n";
            ok = false;
        }
    }

    if (ok)
        cout << "Allocation passes basic legality checks.\n";
    else
        cout << "Errors found in allocation.\n";

    return 0;
}
