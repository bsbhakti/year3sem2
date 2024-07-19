import React from "react";
import "./RecipeForm.css";
import { useState } from "react";
import { MdOutlineFoodBank } from "react-icons/md";
import axios from 'axios';

const RecipeForm = () => {
  const initialData = {
    recipeName: "",
    recipeIngredients: [""],
    recipeDirections: [""],
    cuisine: "",
  };

  const [formData, setFormData] = useState(initialData);

  const handleChange = (event) => {
    const { name, value } = event.target;
    let updatedValue = value
    if(name === "recipeIngredients" || name ==="recipeDirections"){
      updatedValue = value.split(",").map((item) => item.trim(" "));
    }
    setFormData((prevFormData) => ({ ...prevFormData, [name]: updatedValue }));
    // console.log(formData);
  };

  const handleSubmit = (event) => {
    event.preventDefault();
    if (validateForm()) { 
        let date = new Date().toLocaleString();
        console.log(date);
        const data = {
          form: formData,
          date: date
        }
        axios.post("http://localhost:8080/submitForm", data)
        .then(response => {
          
          console.log(response.data.status,"this is response");
          if(response.data.status === 2){
          alert("Successfully updated recipe!");

          }
          else {
          alert("Successfully saved recipe!")
          }
        })
        .catch(err =>{
          alert("Sorry couldnt save recipe!")
        })

        handleReset();
    } else {
      alert("Name, Ingredients and Directions cannot be empty");
    }
  };
  const handleReset = (event) => {
    setFormData(initialData);
    console.log(formData);
  };

  function validateForm() {
    if (
      formData.recipeName.length === 0 ||
      formData.recipeDirections.length === 0 ||
      formData.recipeIngredients.length === 0 ||
      formData.cuisine.length === 0
    ) {
      return false;
    }
    return true;
  }

  return (
    <div className="form-container">
      <form
        className="recipe-form"
      >
        <div className="heading">
          <MdOutlineFoodBank size={35} />
          <h1>New Recipe</h1>
        </div>

        <div className="form-group">
          <label htmlFor="title">Recipe Name</label>
          <input
            type="text"
            id="title"
            name="recipeName"
            value={formData.recipeName}
            onChange={handleChange}
          />
        </div>

        <div className="form-group">
          <label htmlFor="Cuisine">Cuisine</label>
          <select
            id="cuisine"
            name="cuisine"
            value={formData.cuisine}
            onChange={handleChange}
          >
            <option value="" name="cuisine" selected disabled>
              Select Cuisine
            </option>
            <option name="cuisine" value="Italian">
              Italian
            </option>
            <option name="cuisine" value="Mexican">
              Mexican
            </option>
            <option name="cuisine" value="Chinese">
              Chinese
            </option>
            <option name="cuisine" value="Indian">
              Indian
            </option>
            <option name="cuisine" value="French">
              French
            </option>
            <option name="cuisine" value="Canadian">
              Canadian
            </option>
            <option name="cuisine" value="Turkish">
              Turkish
            </option>
            <option name="cuisine" value="Amerian">
              Amerian
            </option>
            <option name="cuisine" value="British">
              British
            </option>
            <option name="cuisine" value="Vietnamese">
              Vietnamese
            </option>
          </select>
        </div>

        <div className="form-group">
          <label htmlFor="ingredients">Ingredients</label>
          <textarea
            id="ingredients"
            name="recipeIngredients"
            value={formData.recipeIngredients}
            onChange={handleChange}
          ></textarea>
        </div>

        <div className="form-group">
          <label htmlFor="directions">Directions</label>
          <textarea
            id="directions"
            name="recipeDirections"
            value={formData.recipeDirections}
            onChange={handleChange}
          ></textarea>
        </div>
        <div className="buttons">
          <div className="form-group">
            <input type="submit" className="submit-button" onClick={handleSubmit}/>
          </div>

          <div className="form-group">
            <input
              type="button"
              className="reset-button"
              onClick={handleReset}
              value="Reset"
            />
          </div>
        </div>
      </form>
    </div>
  );
};

export default RecipeForm;
