class NewWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("New Window with Plot")
        self.setGeometry(100, 100, 600, 400)

        # Create a matplotlib figure and canvas
        self.figure, self.ax = plt.subplots()
        self.canvas = FigureCanvas(self.figure)

        # Example plot
        self.plot_data()

        layout = QVBoxLayout()
        layout.addWidget(self.canvas)
        self.setLayout(layout)

    def plot_data(self):
        # Sample data for plotting
        x = [1, 2, 3, 4, 5]
        y = [2, 3, 5, 7, 11]
        
        # Plotting the data
        self.ax.plot(x, y, marker='o', linestyle='-')
        self.ax.set_title("Sample Plot")
        self.ax.set_xlabel("X-axis")
        self.ax.set_ylabel("Y-axis")
        self.ax.grid()

        # Refresh canvas
        self.canvas.draw()
