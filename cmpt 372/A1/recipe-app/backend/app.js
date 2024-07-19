const { urlencoded } = require("express");
const express = require("express");
const path = require("path");
const { Pool } = require("pg");
var cors = require("cors");
const { connect } = require("http2");

const app = express();
app.use(cors());
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

var pool = new Pool({
  user: "postgres",
  host: "db",
  password: "4123"
});

// app.use(express.static(path.join(__dirname,'..','frontend','build')))

async function getRecipeId(name) {
  const r = await pool.query(`SELECT * from recipes where name = $1`, [name]);
  console.log("recipe id returned by the method ", r.rows[0].id);

  return r.rows[0].id;
}

async function deleteRecipe(recipeId) {
  await pool.query(`DELETE from recipes where id=$1`, [recipeId]);
}

async function deleteRecipeAndRef(recipeId) {
  var ingredientIds = `SELECT ingredientid from recipeingredients where recipeid = $1`;
  var ingredientsIdArr = (await pool.query(ingredientIds, [recipeId])).rows.map(
    (row) => row.ingredientid
  );
  console.log(
    "this is the ingredient array for recipe to delete ",
    ingredientsIdArr
  );

  var countRecipeIngredientRef = `SELECT count(ingredientid) from recipeingredients where ingredientid = $1`;
  for (var ingredient of ingredientsIdArr) {
      console.log(ingredient);
    var refCount = await pool.query(countRecipeIngredientRef, [ingredient]);
    if (refCount.rows[0].count == 1) {
      console.log("deleting ingredient with id ", ingredient);
      await deleteIngredients(ingredient); //this will delete the recipe ingredient too
    } 
  }
    await deleteRecipe(recipeId);
    return 1;

}

async function getIngredientIdFromName(ingredientName) {
  var ingredientId = await pool.query(
    `SELECT id from ingredients where name = $1`,
    [ingredientName]
  );
  console.log("ingredient id returned by the method ", ingredientId.rows[0]);
  return ingredientId.rows[0].id;
}
async function deleteIngredients(ingredientId) {
  var deleteIngredient = `DELETE FROM ingredients where ingredients.id = $1`;
  await pool.query(deleteIngredient, [ingredientId]); //this also deletes the recipeIngredient field
}

async function addRecipes(name, directions, cuisine,date) {
  var addRecipe = `INSERT into recipes(name,directions,cuisine,date) VALUES($1,$2, $3, $4)`;
  var updateRecipe = `UPDATE recipes SET directions = $2,cuisine = $3, date =$4 WHERE name = $1`;
  var checkIfExists = `SELECT COUNT(name) from recipes WHERE recipes.name = $1`;
  var count = await pool.query(checkIfExists, [name]);
  console.log("this is count of recipe name", count.rows[0].count);

  if (count.rows[0].count > 0) {
    // modify and dont insert
    // res.send("Cannot insert it exists");
    console.log("updating recipe", name, directions, cuisine);
    await pool.query(updateRecipe, [name, directions, cuisine,date]);
    return 0;
  } else {
    //insert
    console.log("inserting");
    await pool.query(addRecipe, [name, directions, cuisine, date]);
    return 1;
  }
}

async function addIngredients2(newIngredientArr) {
  console.log("adding ingredients2");
  var checkIfExists = `SELECT COUNT(name) from ingredients WHERE ingredients.name = $1`;
  var ingredientIdsArr = [];

  for (const newIngredientName of newIngredientArr) {
    var count = await pool.query(checkIfExists, [newIngredientName]);
    console.log("this is count on ingredients name", count.rows[0].count);

    if (count.rows[0].count == 0) {
      //doesnt exist
      console.log("inserting ingredient");
      var addIngredients = `INSERT INTO ingredients(name) VALUES($1)`;
      await pool.query(addIngredients, [newIngredientName]);
    }
    ingredientId = await getIngredientIdFromName(newIngredientName);
    ingredientIdsArr.push(ingredientId);
  }

  //   return deleted;
  console.log(
    "this is ingredientIdsArr returned by ingredietAdd2 ",
    ingredientIdsArr
  );

  return ingredientIdsArr;
}

async function addRecipeIngredientsWrapper(
  newIngredientIdsArr,
  recipeId,
  updating
) {
  console.log("this is the new ingredient array ", newIngredientIdsArr);

  if (updating) {
    var oldIngredientsIdQuery = `SELECT ingredientid from recipeingredients where recipeid = $1`;

    var oldIngredientsIdArr = (
      await pool.query(oldIngredientsIdQuery, [recipeId])
    ).rows.map((row) => row.ingredientid);
    console.log("this is the old ingredient array ", oldIngredientsIdArr);

    for (var newIngredientId of newIngredientIdsArr) {
      console.log("goin in not in of the recipe ingredients wrapper");
      if (!oldIngredientsIdArr.includes(newIngredientId)) {
        //not in
        console.log("inside if", newIngredientId, oldIngredientsIdArr);
        await addRecipeIngredientsById(recipeId, newIngredientId);
      } else {
        oldIngredientsIdArr = oldIngredientsIdArr.filter(
          (id) => id != newIngredientId
        );
      }
    }
    console.log(
      "outside the for here is delete old ing arr ",
      oldIngredientsIdArr
    );

    if (oldIngredientsIdArr.length != 0) {
      var countRecipeIngredientRef = `SELECT count(ingredientid) from recipeingredients where ingredientid = $1`;
      for (var remainingIngredientId of oldIngredientsIdArr) {
        var refCount = await pool.query(countRecipeIngredientRef, [
          remainingIngredientId,
        ]);
        if (refCount.rows[0].count == 1) {
          console.log("deleting ingredient with id ", remainingIngredientId);
          await deleteIngredients(remainingIngredientId);
        } else {
          //just delete the recipeIngredient field
          console.log(
            "deleting the recipe ingredient file of ",
            recipeId,
            remainingIngredientId
          );
          var deleteRecipeIngredient = `DELETE FROM recipeIngredients where ingredientid = $1 and recipeid= $2`;
          await pool.query(deleteRecipeIngredient, [
            remainingIngredientId,
            recipeId,
          ]);
        }
      }
    }
  } else {
    //not updating put new entry
    newIngredientIdsArr.forEach(function (ingredientId) {
      addRecipeIngredientsById(recipeId, ingredientId);
    });
  }
}

async function addRecipeIngredientsById(recipeId, ingredientId) {
  console.log("going into add recipe ingredients", ingredientId);
  var addRecipeIngredients = `INSERT into recipeIngredients(recipeid, ingredientid) VALUES( $1,$2 )`;
  await pool.query(addRecipeIngredients, [recipeId, ingredientId]);
}
app.get("/",async (req,res)=>{
  res.send("success");
   const createIngredients = `CREATE TABLE IF NOT EXISTS ingredients(id serial PRIMARY KEY , name VARCHAR(30))`;
  const createRecipes = `CREATE TABLE IF NOT EXISTS recipes(id serial PRIMARY KEY, name VARCHAR(30), directions VARCHAR(200)[], cuisine VARCHAR(10), date VARCHAR(50))`;
  const createRecipeIngredients = `CREATE TABLE IF NOT EXISTS recipeIngredients(recipeid INT, ingredientid INT, FOREIGN KEY(recipeid) REFERENCES recipes(id) ON DELETE CASCADE, FOREIGN KEY(ingredientid) REFERENCES ingredients(id) ON DELETE CASCADE)`;

  await pool.query(createIngredients);
  await pool.query(createRecipes);
  await pool.query(createRecipeIngredients);
})
app.post("/submitForm", async (req, res, next) => {
  console.log("im reaching")
  
  console.log("creating", req.body);
  console.log("creating", req.body.date);

  const createIngredients = `CREATE TABLE IF NOT EXISTS ingredients(id serial PRIMARY KEY , name VARCHAR(30))`;
  const createRecipes = `CREATE TABLE IF NOT EXISTS recipes(id serial PRIMARY KEY, name VARCHAR(30), directions VARCHAR(200)[], cuisine VARCHAR(10), date VARCHAR(50))`;
  const createRecipeIngredients = `CREATE TABLE IF NOT EXISTS recipeIngredients(recipeid INT, ingredientid INT, FOREIGN KEY(recipeid) REFERENCES recipes(id) ON DELETE CASCADE, FOREIGN KEY(ingredientid) REFERENCES ingredients(id) ON DELETE CASCADE)`;

  const recipeName = req.body.form.recipeName;
  const recipeDirections = req.body.form.recipeDirections;
  const recipeCuisine = req.body.form.cuisine;
  const recipeIngredients = req.body.form.recipeIngredients;
  const {date} = req.body;

  await pool.query(createIngredients);
  await pool.query(createRecipes);
  await pool.query(createRecipeIngredients);

  var addResult = await addRecipes(recipeName, recipeDirections, recipeCuisine,date);
  var recipeId = await getRecipeId(recipeName);

  //   var deleted = await addIngredients(recipeIngredients, !addResult, recipeId);
  var newIngredientIdsArr = await addIngredients2(
    recipeIngredients,
    !addResult,
    recipeId
  );
  console.log("new id", newIngredientIdsArr);

  await addRecipeIngredientsWrapper(newIngredientIdsArr, recipeId, !addResult);

  if (!addResult) {
    console.log("recipe updated");
    const status = { status: 2 };
    res.status(200).json(status);
    // res.redirect("/")
  } else {
    console.log("added");
    const status = { status: 1 };
    res.status(200).json(status);

    // res.redirect("/");
  }
});

app.get("/getAllRecipes", async (req, res) => {
    try {
    // Define an array of table names to check
    const tableNames = ['recipes', 'ingredients', 'recipeingredients'];

    // Initialize an object to store the existence status of each table
    const tableExistence = {};

    // Iterate over each table name and check for its existence
    for (const tableName of tableNames) {
      const checkTableQuery = `
        SELECT EXISTS (
          SELECT 1 
          FROM information_schema.tables 
          WHERE table_name = '${tableName}'
        )`;
      const { rows } = await pool.query(checkTableQuery);
      tableExistence[tableName] = rows[0].exists;
    }
    
    const allTablesExist = Object.values(tableExistence).every(
      (exists) => exists
    );

    if(allTablesExist) {
    const getQ = `SELECT 
  id, 
  name,
   ARRAY_AGG(ingredientname) AS ingredients,
   directions,
   cuisine,
   date
  FROM (
      SELECT 
      recipes.id, 
      recipes.name,
       ingredients.name as ingredientName, 
       recipes.directions, 
       recipes.cuisine,
       recipes.date
   FROM  
        ingredients 
    INNER JOIN 
        recipeingredients ON ingredients.id = recipeingredients.ingredientid 
    INNER JOIN 
        recipes ON recipeingredients.recipeid = recipes.id
    ) 
    AS disp_recipes
    GROUP BY id, 
    name, 
    directions, 
    cuisine,
    date  ORDER BY id`;
    var r = await pool.query(getQ);
    // res.json(r)
    //   const arr = []
    console.log("these are all recipes: ", r.rows);
    //   r.rows.map(row=>arr.push(row.name))
    //   console.log(arr);
    res.json(r.rows);
  }
  else {
      console.log("couldntget all tables dont exist")
      res.json([])
  }
}
    catch (error) {
      console.log("couldntget some erorr");

            res.json([])
    }
  
});

app.delete("/deleteRecipe", async (req, res, next) => {
  const idToDelete = req.query.delete.id;
  console.log("delete request received");
  var r = await deleteRecipeAndRef(idToDelete);
  if(r){
  res.status(200).json(r);

  }
  else {
  res.status(400).json(r);

  }
});

// app.listen(port, () => {
//   console.log(`server is listening on port ${port}`);
// });

// app.listen(port, '0.0.0.0');
//   console.log(`server is listening on port ${port}`);

function connectToDatabase() {
    pool.connect((err, client, release) => {
      if (err) {
        console.error(
          "Error connecting to database, retrying in 5 seconds:",
          err
        );
        setTimeout(connectToDatabase, 5000); // for 5 secs
      } else {
        console.log("Database connection successful!");
        app.listen(port, "0.0.0.0");
        console.log(`server is listening on port ${port}`);
      }
    });
  }

connectToDatabase()
