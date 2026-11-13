const staffBody = document.querySelector("#staff tbody");
const projectBody = document.querySelector("#projects tbody");
const studentBody = document.querySelector("#students tbody");
const resultBody = document.querySelector("#result tbody");
const scoreLine = document.getElementById("score");

function field(value, name) {
    const input = document.createElement("input");
    input.value = value;
    input.setAttribute("aria-label", name);
    return input;
}

function addStaff(member = { id: "", load: 1, subjects: [] }) {
    const row = document.createElement("tr");
    [field(member.id, "Staff id"), field(member.load, "Supervision limit"), field(member.subjects.join(" "), "Subjects")].forEach((input) => {
        const cell = document.createElement("td");
        cell.append(input);
        row.append(cell);
    });
    staffBody.append(row);
}

function addProject(project = { id: "", proposer: "", multiplicity: 1, subject: "", title: "" }) {
    const row = document.createElement("tr");
    [field(project.id, "Project id"), field(project.proposer, "Proposer"), field(project.multiplicity, "Places"), field(project.subject, "Subject"), field(project.title, "Title")].forEach((input) => {
        const cell = document.createElement("td");
        cell.append(input);
        row.append(cell);
    });
    projectBody.append(row);
}

function addStudent(student = { id: "", choices: [] }) {
    const row = document.createElement("tr");
    const values = [student.id, student.choices[0] || "", student.choices[1] || "", student.choices[2] || "", student.choices[3] || ""];
    const labels = ["Student id", "First choice", "Second choice", "Third choice", "Fourth choice"];
    values.forEach((value, index) => {
        const cell = document.createElement("td");
        cell.append(field(value, labels[index]));
        row.append(cell);
    });
    studentBody.append(row);
}

function readRows(body, names) {
    return [...body.querySelectorAll("tr")].map((row) => {
        const values = [...row.querySelectorAll("input")].map((input) => input.value.trim());
        if (values.every((value) => value === "")) return null;
        const record = {};
        names.forEach((name, index) => {
            record[name] = values[index] || "";
        });
        return record;
    }).filter(Boolean);
}

function currentInput() {
    const staff = readRows(staffBody, ["id", "load", "subjects"]).map((member) => ({
        id: member.id,
        load: Number(member.load),
        subjects: member.subjects.split(/\s+/).filter(Boolean)
    }));
    const projects = readRows(projectBody, ["id", "proposer", "multiplicity", "subject", "title"]).map((project) => ({
        id: Number(project.id),
        proposer: project.proposer,
        multiplicity: Number(project.multiplicity),
        subject: project.subject,
        title: project.title
    }));
    const students = readRows(studentBody, ["id", "first", "second", "third", "fourth"]).map((student) => ({
        id: student.id,
        choices: [student.first, student.second, student.third, student.fourth].filter(Boolean).map(Number)
    }));
    return { staff, projects, students };
}

function showResult() {
    const { staff, projects, students } = currentInput();
    const { score, rows } = allocate(staff, projects, students);
    scoreLine.textContent = `Score ${score}`;
    resultBody.replaceChildren();
    rows.forEach((row) => {
        const line = document.createElement("tr");
        const reason = row.projectId === -1
            ? "No project"
            : `${row.choiceScore} for the choice, ${row.supervisorScore} for the supervisor`;
        [row.studentId, row.title || "—", row.supervisorId || "—", reason].forEach((text) => {
            const cell = document.createElement("td");
            cell.textContent = text;
            line.append(cell);
        });
        resultBody.append(line);
    });
}

function loadExample(name) {
    const example = examples[name];
    staffBody.replaceChildren();
    projectBody.replaceChildren();
    studentBody.replaceChildren();
    example.staff.forEach(addStaff);
    example.projects.forEach(addProject);
    example.students.forEach(addStudent);
    showResult();
}

document.getElementById("example-small").addEventListener("click", () => loadExample("small"));
document.getElementById("example-busy").addEventListener("click", () => loadExample("busy"));
document.getElementById("add-staff").addEventListener("click", () => addStaff());
document.getElementById("add-project").addEventListener("click", () => addProject());
document.getElementById("add-student").addEventListener("click", () => addStudent());
document.getElementById("allocate").addEventListener("click", showResult);
loadExample("small");
