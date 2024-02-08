CREATE TABLE users (
    user_id SERIAL PRIMARY KEY,
    user_name VARCHAR(50) NOT NULL,
    password VARCHAR(127) NOT NULL,
    introduction VARCHAR(127) NOT NULL,
    create_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    -- とりあえずパスワードログインの形に
);