CREATE TABLE results (
    result_id SERIAL PRIMARY KEY,
    user_id INTEGER REFERENCES users(user_id),
    code_id INTEGER REFERENCES codes(code_id),
    answer_id INTEGER REFERENCES answers(answer_id),
    result TEXT NOT NULL,
    create_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);