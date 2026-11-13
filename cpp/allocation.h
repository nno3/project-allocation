#ifndef ALLOCATION_H
#define ALLOCATION_H

#include <string>
#include <vector>
#include <map>

struct Staff
{
    std::string id;
    int load = 0;                      // load: maximum supervision load
    std::vector<std::string> subjects; // subjects: areas of expertise
};

struct Project
{
    int id = -1;
    std::string proposer; // proposer: staff member who proposed it
    int multiplicity = 0; // multiplicity: how many students may take it
    std::string subject;
    std::string title;
};

struct Student
{
    std::string id;
    std::vector<int> choices; // upto 4 project ids
};

struct Allocation
{
    int projectId = -1;
    std::string supervisorId; // empty if none yet
};

// convenient map typedefs for clarity
using StaffMap = std::map<std::string, Staff>;
using ProjectMap = std::map<int, Project>;
using StudentMap = std::map<std::string, Student>;
using AllocMap = std::map<std::string, Allocation>;

// reading input
void readStaff(const std::string &filename, StaffMap &staffMap);
void readProjects(const std::string &filename, ProjectMap &projectMap);
void readStudents(const std::string &filename, StudentMap &studentMap, std::vector<std::string> &studentOrder);

// core allocation steps
void phase1(const StudentMap &students, const std::vector<std::string> &studentOrder, const ProjectMap &projects, AllocMap &alloc, std::map<int, int> &projectCount);

void phase2_1(const StaffMap &staffMap, const ProjectMap &projects, const StudentMap &students, const std::vector<std::string> &studentOrder, AllocMap &alloc, std::map<int, int> &projectCount, std::map<std::string, int> &staffCount);

void phase2_2(const StaffMap &staffMap, const ProjectMap &projects, const StudentMap &students, const std::vector<std::string> &studentOrder, AllocMap &alloc, std::map<int, int> &projectCount, std::map<std::string, int> &staffCount);

void phase2_3(const StaffMap &staffMap, const ProjectMap &projects,
              const StudentMap &students, const std::vector<std::string> &studentOrder, AllocMap &alloc, std::map<int, int> &projectCount, std::map<std::string, int> &staffCount);

// scoring helpers
int studentPreferenceScore(const Student &stu, int projectId);
int supervisorPreferenceScore(const Staff &staff, const Project &project);
int computeTotalScore(const StaffMap &staffMap, const ProjectMap &projects, const StudentMap &students, const AllocMap &alloc);

// utility
bool staffHasSubject(const Staff &staff, const std::string &subject);

#endif
