CREATE TABLE IF NOT EXISTS users (
    id INT PRIMARY KEY,
    email TEXT UNIQUE NOT NULL,
    firstName TEXT NOT NULL,
    lastName TEXT NOT NULL,
    passHash TEXT NOT NULL,
    misc TEXT
);

CREATE TABLE IF NOT EXISTS urls (
    ownerID INT NOT NULL,
    hashStr VARCHAR(16) NOT NULL,
    url VARCHAR(255) NOT NULL
);