from matplotlib.patches import Polygon

class DraggableBox:
    def __init__(self, ax):
        self.ax = ax

        self.line1 = self.ax.axvline(x=0, color="red", linestyle="-")
        self.line2 = self.ax.axvline(x=2000, color="red", linestyle="-")
        self.red_bg = Polygon([(0, ax.get_ylim()[0]), (0, ax.get_ylim()[1]), 
                               (2000, ax.get_ylim()[1]), (2000, ax.get_ylim()[0])], 
                              closed=True, color='red', alpha=0.3)
        self.ax.add_patch(self.red_bg)

        self.dragging = False
        self.start_x = 0
        self.start_pos = None

    def on_press(self, event):
        if event.inaxes == self.line1.axes:
            x1 = self.line1.get_xdata()[0]
            x2 = self.line2.get_xdata()[0]
            x = event.xdata
            if x1 <= x <= x2:
                self.dragging = True
                self.start_x = event.xdata
                self.start_pos = (self.line1.get_xdata()[0], self.line2.get_xdata()[0])

    def on_release(self, event):
        self.dragging = False

    def on_motion(self, event):
        if self.dragging:
            xdata = event.xdata
            if xdata is not None:
                # Calculate new positions
                delta_x = xdata - self.start_x
                new_x_start = self.start_pos[0] + delta_x
                new_x_end = self.start_pos[1] + delta_x

                # Update lines
                self.line1.set_xdata([new_x_start, new_x_start])
                self.line2.set_xdata([new_x_end, new_x_end])
                
                self.red_bg.set_xy([(new_x_start, self.ax.get_ylim()[0]),
                                    (new_x_start, self.ax.get_ylim()[1]),
                                    (new_x_end, self.ax.get_ylim()[1]),
                                    (new_x_end, self.ax.get_ylim()[0])])
                
                self.line1.figure.canvas.draw()

                self.line1.figure.canvas.draw()