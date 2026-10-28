INSERT OR IGNORE INTO ingredients (name, unit, userId) VALUES
    ('Mąka pszenna', 'g', 0),
    ('Mleko', 'ml', 0),
    ('Jajka', 'szt.', 0),
    ('Masło', 'g', 0),
    ('Cukier', 'g', 0),
    ('Sól', 'g', 0),
    ('Olej rzepakowy', 'ml', 0),
    ('Płatki owsiane', 'g', 0),
    ('Banan', 'szt.', 0),
    ('Jogurt naturalny', 'g', 0),
    ('Ryż', 'g', 0),
    ('Pierś z kurczaka', 'g', 0),
    ('Brokuł', 'g', 0),
    ('Makaron', 'g', 0),
    ('Passata pomidorowa', 'ml', 0),
    ('Cebula', 'szt.', 0),
    ('Pomidor', 'szt.', 0),
    ('Ogórek', 'szt.', 0),
    ('Ser feta', 'g', 0),
    ('Oliwa z oliwek', 'ml', 0);

-- Approximate energy for one portion represented by each recipe.
INSERT OR IGNORE INTO crafting (name, kcal, userId) VALUES
    ('Naleśniki', 530, 0),
    ('Owsianka z bananem', 440, 0),
    ('Jajecznica', 310, 0),
    ('Ryż z kurczakiem i brokułem', 620, 0),
    ('Makaron w sosie pomidorowym', 520, 0),
    ('Sałatka z fetą', 390, 0);

WITH portions(recipe, ingredient, quantity) AS (VALUES
    ('Naleśniki', 'Mąka pszenna', 80),
    ('Naleśniki', 'Mleko', 150),
    ('Naleśniki', 'Jajka', 1),
    ('Naleśniki', 'Olej rzepakowy', 5),
    ('Owsianka z bananem', 'Płatki owsiane', 60),
    ('Owsianka z bananem', 'Mleko', 200),
    ('Owsianka z bananem', 'Banan', 1),
    ('Jajecznica', 'Jajka', 3),
    ('Jajecznica', 'Masło', 10),
    ('Jajecznica', 'Sól', 1),
    ('Ryż z kurczakiem i brokułem', 'Ryż', 80),
    ('Ryż z kurczakiem i brokułem', 'Pierś z kurczaka', 150),
    ('Ryż z kurczakiem i brokułem', 'Brokuł', 150),
    ('Ryż z kurczakiem i brokułem', 'Olej rzepakowy', 10),
    ('Makaron w sosie pomidorowym', 'Makaron', 100),
    ('Makaron w sosie pomidorowym', 'Passata pomidorowa', 200),
    ('Makaron w sosie pomidorowym', 'Cebula', 1),
    ('Makaron w sosie pomidorowym', 'Oliwa z oliwek', 10),
    ('Sałatka z fetą', 'Pomidor', 2),
    ('Sałatka z fetą', 'Ogórek', 1),
    ('Sałatka z fetą', 'Ser feta', 80),
    ('Sałatka z fetą', 'Oliwa z oliwek', 10)
)
INSERT OR IGNORE INTO crafting_ingredients (ingredientId, craftingId, quantity, userId)
SELECT i.id, c.id, p.quantity, 0
FROM portions p
JOIN ingredients i ON i.name = p.ingredient AND i.userId = 0
JOIN crafting c ON c.name = p.recipe AND c.userId = 0;
