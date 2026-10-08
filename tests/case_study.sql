-- Case Study: University Department & Student Academic Tracking System
-- Demonstrates multi-table schema creation, data insertion, and sequential multi-query analytical reporting.

-- 1. Create Schemas
CREATE TABLE departments (dept_id, dept_name, budget);
CREATE TABLE courses (course_id, course_name, dept_id, credits);
CREATE TABLE students (student_id, name, dept_id, gpa, year);

-- 2. Populate Departments Data
INSERT INTO departments VALUES (1, 'Computer Science', 500000);
INSERT INTO departments VALUES (2, 'Electrical Engineering', 450000);
INSERT INTO departments VALUES (3, 'Mathematics', 300000);

-- 3. Populate Courses Data
INSERT INTO courses VALUES (101, 'Data Structures', 1, 4);
INSERT INTO courses VALUES (102, 'Algorithms', 1, 4);
INSERT INTO courses VALUES (103, 'Linear Algebra', 3, 3);
INSERT INTO courses VALUES (104, 'Circuit Analysis', 2, 4);
INSERT INTO courses VALUES (105, 'Discrete Math', 3, 2);

-- 4. Populate Students Data
INSERT INTO students VALUES (1001, 'Alice Johnson', 1, 3.85, 3);
INSERT INTO students VALUES (1002, 'Bob Smith', 1, 3.40, 2);
INSERT INTO students VALUES (1003, 'Charlie Brown', 2, 3.92, 4);
INSERT INTO students VALUES (1004, 'Diana Prince', 3, 3.75, 3);
INSERT INTO students VALUES (1005, 'Evan Wright', 1, 2.90, 1);
INSERT INTO students VALUES (1006, 'Fiona Gallagher', 2, 3.60, 4);

-- -------------------------------------------------------------
-- Analytical Queries
-- -------------------------------------------------------------

-- Query 1: Department Overview ordered by Budget
SELECT dept_id, dept_name, budget FROM departments ORDER BY budget;

-- Query 2: High Credit Courses (credits >= 4)
SELECT course_name, dept_id, credits FROM courses WHERE credits >= 4 ORDER BY course_name;

-- Query 3: Honor Roll Senior Students (GPA >= 3.5 AND year >= 3) ordered by GPA
SELECT name, dept_id, gpa, year FROM students WHERE gpa >= 3.5 AND year >= 3 ORDER BY gpa;

-- Query 4: Computer Science Students (dept_id = 1) ordered by name
SELECT student_id, name, gpa FROM students WHERE dept_id = 1 ORDER BY name;
