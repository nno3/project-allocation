function studentPoints(choices, projectId) {
    const index = choices.indexOf(projectId);
    return index === -1 ? 0 : 4 - index;
}

function supervisorPoints(staff, project) {
    if (project.proposer === staff.id) return 4;
    if (staff.subjects.includes(project.subject)) return 2;
    return 0;
}

function allocate(staffList, projectList, studentList) {
    const staff = new Map(staffList.map((member) => [member.id, member]));
    const projects = new Map(projectList.map((project) => [project.id, project]));
    const students = new Map(studentList.map((student) => [student.id, student]));
    const staffIds = [...staff.keys()].sort();
    const projectIds = [...projects.keys()].sort((left, right) => left - right);
    const studentOrder = [...students.keys()].sort();
    const alloc = new Map(studentOrder.map((id) => [id, { projectId: -1, supervisorId: "" }]));
    const projectCount = new Map(projectIds.map((id) => [id, 0]));
    const staffCount = new Map(staffIds.map((id) => [id, 0]));

    for (let preference = 0; preference < 4; preference += 1) {
        studentOrder.forEach((id) => {
            const assignment = alloc.get(id);
            if (assignment.projectId !== -1) return;
            const choices = students.get(id).choices;
            if (preference >= choices.length) return;
            const projectId = choices[preference];
            const project = projects.get(projectId);
            if (!project) return;
            if (projectCount.get(projectId) < project.multiplicity) {
                assignment.projectId = projectId;
                projectCount.set(projectId, projectCount.get(projectId) + 1);
            }
        });
    }

    function assignExisting(staffId, projectId) {
        const load = staff.get(staffId).load;
        studentOrder.forEach((id) => {
            if (staffCount.get(staffId) >= load) return;
            const assignment = alloc.get(id);
            if (assignment.projectId === projectId && assignment.supervisorId === "") {
                assignment.supervisorId = staffId;
                staffCount.set(staffId, staffCount.get(staffId) + 1);
            }
        });
    }

    function assignUnallocated(staffId, projectId) {
        const load = staff.get(staffId).load;
        const capacity = projects.get(projectId).multiplicity;
        studentOrder.forEach((id) => {
            if (staffCount.get(staffId) >= load) return;
            const assignment = alloc.get(id);
            if (assignment.projectId !== -1) return;
            if (projectCount.get(projectId) >= capacity) return;
            assignment.projectId = projectId;
            assignment.supervisorId = staffId;
            projectCount.set(projectId, projectCount.get(projectId) + 1);
            staffCount.set(staffId, staffCount.get(staffId) + 1);
        });
    }

    function phase(accepts) {
        staffIds.forEach((staffId) => {
            projectIds.forEach((projectId) => {
                const project = projects.get(projectId);
                if (!accepts(staffId, project)) return;
                assignExisting(staffId, projectId);
                if (projectCount.get(projectId) < project.multiplicity) assignUnallocated(staffId, projectId);
            });
        });
    }

    phase((staffId, project) => project.proposer === staffId);
    phase((staffId, project) => project.proposer !== staffId && staff.get(staffId).subjects.includes(project.subject));
    phase(() => true);

    const rows = studentOrder.map((id) => {
        const assignment = alloc.get(id);
        const project = projects.get(assignment.projectId);
        const member = staff.get(assignment.supervisorId);
        const choiceScore = project ? studentPoints(students.get(id).choices, assignment.projectId) : 0;
        const supervisorScore = project && member ? supervisorPoints(member, project) : 0;
        return {
            studentId: id,
            projectId: assignment.projectId,
            title: project ? project.title : "",
            supervisorId: assignment.supervisorId,
            choiceScore,
            supervisorScore
        };
    });
    const score = rows.reduce((total, row) => total + row.choiceScore + row.supervisorScore, 0);
    return { score, rows };
}

const examples = {
    small: {
        staff: [
            { id: "staff1", load: 1, subjects: ["SW"] },
            { id: "staff2", load: 1, subjects: ["SW"] },
            { id: "staff3", load: 1, subjects: ["SW"] },
            { id: "staff4", load: 1, subjects: ["SW"] }
        ],
        projects: [
            { id: 1, proposer: "staff1", multiplicity: 1, subject: "SW", title: "helloworld" },
            { id: 2, proposer: "staff2", multiplicity: 1, subject: "SW", title: "easy peasy" },
            { id: 3, proposer: "staff3", multiplicity: 1, subject: "SW", title: "a piece of cake" },
            { id: 4, proposer: "staff4", multiplicity: 1, subject: "SW", title: "a login page" }
        ],
        students: [
            { id: "stu1", choices: [1, 2, 3, 4] },
            { id: "stu2", choices: [2, 3, 4, 1] },
            { id: "stu3", choices: [3, 4, 1, 2] },
            { id: "stu4", choices: [4, 1, 2, 3] }
        ]
    },
    busy: {
        staff: [
            { id: "staff1", load: 1, subjects: ["SW"] },
            { id: "staff2", load: 2, subjects: ["SW"] },
            { id: "staff3", load: 1, subjects: ["SW"] },
            { id: "staff4", load: 1, subjects: ["SW"] }
        ],
        projects: [
            { id: 1, proposer: "staff1", multiplicity: 1, subject: "SW", title: "helloworld" },
            { id: 2, proposer: "staff2", multiplicity: 5, subject: "SW", title: "easy peasy" },
            { id: 3, proposer: "staff3", multiplicity: 1, subject: "SW", title: "a piece of cake" },
            { id: 4, proposer: "staff4", multiplicity: 1, subject: "SW", title: "a login page" }
        ],
        students: [
            { id: "stu1", choices: [2, 1, 3, 4] },
            { id: "stu2", choices: [2, 3, 4, 1] },
            { id: "stu3", choices: [2, 4, 1, 3] },
            { id: "stu4", choices: [2, 4, 3, 1] },
            { id: "stu5", choices: [4, 1, 2, 3] }
        ]
    }
};

if (typeof module !== "undefined") module.exports = { allocate, examples };
