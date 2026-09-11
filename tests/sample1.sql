-- Sample SQL queries for minisql (Scoped Subset)

CREATE TABLE students (id, name, age);

INSERT INTO students VALUES (1, 'Alice', 20);

SELECT name, age FROM students WHERE age >= 20 AND age <= 30 ORDER BY name;

SELECT * FROM students;
