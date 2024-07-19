import "./App.css";
import RecipeForm from "./RecipeForm/RecipeForm";
import RecipeList from "./RecipeList/RecipeList";
import RecipePopup from "./RecipePopup/RecipePopup";
import Navbar from "./Navbar/Navbar";
import { useState } from "react";
import { BrowserRouter, BrowserRouter as Router, Route, Routes } from "react-router-dom";

function App() {
  const [selectedRecipe, setSelectedRecipe] = useState(null);
  const [isModalOpen, setIsModalOpen] = useState(false);

  const handleRecipeClick = (recipe) => {
    console.log("look here", recipe);
    setSelectedRecipe(recipe);
    setIsModalOpen(true);
  };

  const handleClosePopup = () => {
    setSelectedRecipe(null);
    setIsModalOpen(false);
  };

  return (
    <div className="App">
      <Navbar />
      <Routes>
        <Route path="/" element={<RecipeForm></RecipeForm>}></Route>
        <Route path="/recipeList" element={<RecipeList onRecipeClick={handleRecipeClick} isModalOpen={isModalOpen}/>}></Route>
      </Routes>
      <RecipePopup recipe={selectedRecipe} isOpen={isModalOpen} onClose={handleClosePopup}/>
    </div>
  );
}

export default App;
