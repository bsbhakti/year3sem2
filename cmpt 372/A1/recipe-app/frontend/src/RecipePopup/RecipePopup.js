import React from 'react';
import './RecipePopup.css'
import { FaTimes } from "react-icons/fa";
import axios from 'axios';
import { redirect } from 'react-router';


const RecipePopup = ({ isOpen,recipe, onClose }) => {
  if (!isOpen) return null

  const deleteButtonClicked = () =>{
    const params = {delete:recipe}
    console.log("paras",params);
    axios
      .delete("http://localhost:8080/deleteRecipe", { params })
      .then((res) => {
        if(res.status ==200){
        alert("Recipe Deleted");
        onClose();
        console.log("open: ",isOpen);
        }
      })
      .catch((err) => {
        onClose();
        alert("Sorry couldnt delete recipe!");
      });
  }


  return (
    <div className="modal-overlay">
      <div className="modal-content">
        <div className="header-container" onClick={onClose}>
          <FaTimes />
        </div>
        <h2>{recipe.name}</h2>
        Modified on: {recipe.date}
        <div className="list-header">Cuisine: {recipe.cuisine}</div>
        <ol className="list-header">
          {/* Ingredients:{" "} */}
          
          {recipe.ingredients.map((ingredient, index) => (
            <li className="list-item" key={index}>
              {index + 1}. {ingredient}{" "}
            </li>
          ))}
        </ol>
        <ol className="list-header">
          Directions:{" "}
          {recipe.directions.map((direction, index) => (
            <li className="list-item" key={index}>
              {index + 1}. {direction}{" "}
            </li>
          ))}
        </ol>
        <button onClick={deleteButtonClicked}> Delete </button>
      </div>
    </div>
  );
}

RecipePopup.propTypes = {};

RecipePopup.defaultProps = {};

export default RecipePopup;
