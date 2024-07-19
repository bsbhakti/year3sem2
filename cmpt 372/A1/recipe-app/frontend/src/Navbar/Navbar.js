import React from 'react';
import { Link } from 'react-router-dom';
import "./Navbar.css"


const Navbar = () => (
  <nav>
    <ul>
      <li>
        <Link to="/" className="navbar-text">
          Add Recipe
        </Link>
      </li>
      <li>
        <Link to="/recipeList" className="navbar-text">
          Recipes
        </Link>
      </li>
    </ul>
  </nav>
);

Navbar.propTypes = {};

Navbar.defaultProps = {};

export default Navbar;
