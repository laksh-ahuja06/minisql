-- CRUD Test Script: Demonstrates Full Create, Read, Update, Delete Lifecycle
CREATE TABLE employees (id, name, department, salary);

-- 1. Create / Insert records
INSERT INTO employees VALUES (1, 'Alice', 'Engineering', 75000);
INSERT INTO employees VALUES (2, 'Bob', 'Marketing', 50000);
INSERT INTO employees VALUES (3, 'Charlie', 'Engineering', 68000);
INSERT INTO employees VALUES (4, 'David', 'Sales', 55000);
INSERT INTO employees VALUES (5, 'Eve', 'Marketing', 52000);

-- Initial view of all records
SELECT * FROM employees;

-- 2. Update records
-- Give Charlie a raise
UPDATE employees SET salary = 72000 WHERE name = 'Charlie';

-- Move Bob to Sales
UPDATE employees SET department = 'Sales' WHERE name = 'Bob';

-- View employees after updates
SELECT * FROM employees;

-- 3. Delete records
-- Delete employees in Sales with salary <= 50000
DELETE FROM employees WHERE department = 'Sales' AND salary <= 50000;

-- Delete Alice
DELETE FROM employees WHERE name = 'Alice';

-- View remaining employees ordered by salary
SELECT name, department, salary FROM employees ORDER BY salary;
