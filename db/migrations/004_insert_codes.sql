INSERT INTO codes (user_id, parent_id, code_data, title, detail)
VALUES
    (1, NULL, 'printf("Hello, world!")', 'Hello World', 'Pythonでハローワールドができない'),
    (1, NULL, 'SELECT * FROM users;', 'Select Users', 'Query to select all users from the users table.'),
    (2, 1, 'print("Hello, world!")', 'Hello World2', 'なおしたよ');