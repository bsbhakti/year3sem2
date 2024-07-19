import React from "react";
import "./RecipeList.css";
import { useState, useEffect } from "react";
import axios from "axios";

const RecipeList = ({ onRecipeClick, isModalOpen }) => {
  const [recipes, setRecipes] = useState([]);
  const [state, setState] = useState({
    query: "",
    list: recipes,
  });

  const reloadRecipes = () => {
    axios
      .get("http://localhost:8080/getAllRecipes")
      .then((response) => {
        console.log("this is response", response.data);
        setRecipes(response.data);
        console.log(recipes);
      })
      .catch((err) => {
        console.log("Error getting entries");
        alert("Sorry cannot get entries");
      });
  };

  useEffect(() => {
    //Runs only on the first render
    reloadRecipes()
  }, []);

  useEffect(()=>{
    reloadRecipes()
  },[isModalOpen]);

  useEffect(() => {
    setState({
      list: recipes,
    });
    console.log(recipes);
  }, [recipes]);

  const handleQueryChange = (event) => {
    const results = recipes.filter((recipe) => {
      if (event.target.value === "") return recipes;
      return recipe.recipeName
        .toLowerCase()
        .includes(event.target.value.toLowerCase());
    });

    setState({
      query: event.target.value,
      list: results,
    });

    console.log(state.list);
  };

  return (
    <div>
      {(recipes.length > 0 && (
        <div>
          <div>
            <form className="search_bar">
              <input
                type="search"
                value={state.query}
                placeholder="Search"
                onChange={handleQueryChange}
              />
            </form>
          </div>
          <table>
            <thead>
              <tr>
                <th> Saved Recipes</th>
              </tr>
            </thead>
            <tbody>
              {state.list.map((recipe) => (
                <tr key={recipe.id} onClick={() => onRecipeClick(recipe)}>
                  <td>{recipe.name}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      )) ||
        (recipes.length === 0 && (
          <div>
            <form>
              <input
                type="search"
                value={state.query}
                onChange={handleQueryChange}
              />
            </form>
            <h1> No recipes saved!</h1>
          </div>
        ))}
    </div>
  );
};

RecipeList.propTypes = {};

RecipeList.defaultProps = {};

export default RecipeList;
