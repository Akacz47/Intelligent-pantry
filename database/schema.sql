CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    login TEXT NOT NULL UNIQUE,
    oldPassword TEXT NOT NULL,
    currentPassword TEXT NOT NULL,
    kcal INTEGER NOT NULL DEFAULT 2000,
    rfidBool INTEGER NOT NULL DEFAULT 0 CHECK (rfidBool IN (0, 1))
);

-- userId = 0 denotes shared ingredients and recipes, not a login account.
CREATE TABLE IF NOT EXISTS ingredients (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL COLLATE NOCASE,
    unit TEXT NOT NULL,
    userId INTEGER NOT NULL,
    UNIQUE (userId, name)
);

CREATE TABLE IF NOT EXISTS crafting (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL COLLATE NOCASE,
    kcal INTEGER NOT NULL,
    userId INTEGER NOT NULL,
    UNIQUE (userId, name)
);

CREATE TABLE IF NOT EXISTS crafting_ingredients (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    ingredientId INTEGER NOT NULL REFERENCES ingredients(id),
    craftingId INTEGER NOT NULL REFERENCES crafting(id) ON DELETE CASCADE,
    quantity INTEGER NOT NULL CHECK (quantity > 0),
    userId INTEGER NOT NULL,
    UNIQUE (craftingId, ingredientId)
);

CREATE TABLE IF NOT EXISTS resources (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    quantity INTEGER NOT NULL CHECK (quantity >= 0),
    userId INTEGER NOT NULL REFERENCES users(id),
    ingrOrCraftId INTEGER NOT NULL,
    ingrOrCraftBool INTEGER NOT NULL CHECK (ingrOrCraftBool IN (0, 1)),
    UNIQUE (userId, ingrOrCraftId, ingrOrCraftBool)
);

-- EPC tag identifiers are hexadecimal strings, so retain leading zeros.
CREATE TABLE IF NOT EXISTS rfid (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    tagId TEXT NOT NULL,
    ingrOrCraftId INTEGER NOT NULL,
    ingrOrCraftBool INTEGER NOT NULL CHECK (ingrOrCraftBool IN (0, 1)),
    userId INTEGER NOT NULL REFERENCES users(id),
    UNIQUE (userId, tagId)
);
