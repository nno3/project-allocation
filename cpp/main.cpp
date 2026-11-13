#include "allocation.h"

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

// Program usage: ./GenAlloc staff.txt projects.txt students.txt alloc.txt
int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        cerr << "Usage: " << argv[0]
             << " staff.txt projects.txt students.txt alloc.txt\n";
        return 1;
    }

    string staffFile = argv[1];
    string projectFile = argv[2];
    string studentFile = argv[3];
    string outputFile = argv[4];

    StaffMap staffMap;
    ProjectMap projectMap;
    StudentMap studentMap;
    vector<string> studentOrder; // sort in alphabetical order

    // load all input data
    readStaff(staffFile, staffMap);
    readProjects(projectFile, projectMap);
    readStudents(studentFile, studentMap, studentOrder);

    AllocMap alloc;
    map<int, int> projectCount;
    map<string, int> staffCount;

    // Initialise staff load counters
    for (const auto &sEntry : staffMap)
    {
        staffCount[sEntry.first] = 0;
    }

    // Phase 1: allocate projects (no supervisors yet)
    phase1(studentMap, studentOrder, projectMap, alloc, projectCount);

    // Phase 2.1: staff to own projects
    phase2_1(staffMap, projectMap, studentMap, studentOrder,
             alloc, projectCount, staffCount);

    // Phase 2.2: staff to projects in their subject areas
    phase2_2(staffMap, projectMap, studentMap, studentOrder,
             alloc, projectCount, staffCount);

    // Phase 2.3: staff to any remaining projects
    phase2_3(staffMap, projectMap, studentMap, studentOrder,
             alloc, projectCount, staffCount);

    int score = computeTotalScore(staffMap, projectMap, studentMap, alloc);

    // Output: sorted by student id
    vector<string> sortedIds;
    for (const auto &entry : studentMap)
    {
        sortedIds.push_back(entry.first);
    }
    sort(sortedIds.begin(), sortedIds.end());

    ofstream out(outputFile.c_str());
    if (!out)
    {
        cerr << "Cannot open output file: " << outputFile << "\n";
        return 1;
    }

    for (const string &sid : sortedIds)
    {
        Allocation a = alloc[sid];
        out << sid << " " << a.projectId << " " << a.supervisorId << "\n";
    }
    out << score << "\n";

    return 0;
}
