CREATE TABLE answers (
    answer_id SERIAL PRIMARY KEY,
    user_id INTEGER REFERENCES users(user_id),
    code_id INTEGER REFERENCES codes(code_id),
    indata TEXT NOT NULL,
    outdata TEXT NOT NULL,
    create_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);