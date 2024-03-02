CREATE TABLE IF NOT EXISTS users (
    id INT PRIMARY KEY,
    email TEXT UNIQUE NOT NULL,
    firstName TEXT NOT NULL,
    lastName TEXT NOT NULL,
    passHash TEXT NOT NULL,
    misc TEXT
)