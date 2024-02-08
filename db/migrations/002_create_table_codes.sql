CREATE TABLE codes (
    code_id SERIAL PRIMARY KEY,
    user_id INTEGER REFERENCES users(user_id),
    parent_id INTEGER REFERENCES codes(code_id),
    code_data TEXT NOT NULL,
    title VARCHAR(255),
    detail TEXT NOT NULL,
    create_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);