const SimpleComponent = () => {
  const [count, setCount] = React.useState(0);
  const [isHovered, setIsHovered] = React.useState(false);

  const buttonStyle = {
    padding: "12px 24px",
    borderRadius: "8px",
    backgroundColor: isHovered ? "#4CAF50" : "#367c39",
    color: "black",
    fontSize: "16px",
    margin: "10px",
    cursor: "pointer",
    border: "none",
    boxShadow: isHovered
      ? "0 8px 8px rgba(255, 255, 255, 0.1)"
      : "0 4px 4px rgba(255, 255, 255, 0.2)",
    transition:
      "background-color 0.3s ease, transform 0.2s ease, box-shadow 0.3s ease",
    transform: isHovered ? "scale(1.05)" : "scale(1)",
  };

  const containerStyle = {
    display: "flex",
    flexDirection: "column",
    alignItems: "center",
    justifyContent: "center",
    height: "100%",
  };

  const textStyle = {
    fontSize: "18px",
    color: "#fff",
    margin: "10px 0",
  };

  const countStyle = {
    fontSize: "24px",
    fontWeight: "bold",
    color: "#2196F3",
    margin: "10px 0",
  };

  return React.createElement(
    "div",
    { style: containerStyle },
    React.createElement("h2", { style: textStyle }, "My Simple Component"),
    React.createElement("p", { style: textStyle }, "Click the button:"),
    React.createElement("div", { style: countStyle }, count),
    React.createElement(
      "button",
      {
        style: buttonStyle,
        onClick: () => setCount(count + 1),
        onMouseEnter: () => setIsHovered(true),
        onMouseLeave: () => setIsHovered(false),
      },
      "Increment",
    ),
  );
};

const rootElement = document.getElementById("react-root");
const root = ReactDOM.createRoot(rootElement);
root.render(React.createElement(SimpleComponent));
