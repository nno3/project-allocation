# Project allocation

Allocates final-year students to projects and supervisors.

Partner project with Ayan Muday for CO3105. We built it closely together.

Staff propose projects. Each member of staff has a supervision limit and one or more subject areas. Each project belongs to one subject and can be taken by a set number of students. Each student ranks up to four projects.

The program gives each student the highest choice that still has space. It then assigns supervisors in three steps: the member of staff who proposed the project, then someone in that subject, then anyone who still has room. A first choice scores 4, then 3, 2, and 1. A supervisor scores 4 for their own project, 2 for a project in their subject, and 0 otherwise.

Live page: https://nno3.github.io/project-allocation/

The C++ source is in `cpp/`. A small example, where every student gets their first choice, is in `example/`.
