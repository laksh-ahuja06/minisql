-- Comprehensive MiniSQL end-to-end execution test
CREATE TABLE students (id, name, age, gpa);

INSERT INTO students VALUES (1, 'Alice', 22, 3.8);
INSERT INTO students VALUES (2, 'Bob', 19, 3.2);
INSERT INTO students VALUES (3, 'Charlie', 24, 3.9);
INSERT INTO students VALUES (4, 'David', 20, 2.9);
INSERT INTO students VALUES (5, 'Eve', 22, 3.5);

-- Query 1: Select all records
SELECT * FROM students;

-- Query 2: Filter with compound WHERE (AND) and ORDER BY gpa
SELECT name, age, gpa FROM students WHERE age >= 20 AND gpa > 3.4 ORDER BY gpa;

-- Query 3: Filter with compound WHERE (OR) and ORDER BY age
SELECT name, age FROM students WHERE age < 20 OR age > 23 ORDER BY age;
