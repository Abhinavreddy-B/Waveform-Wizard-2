import matplotlib.pyplot as plt
import numpy as np
import copy

def copy_axis_properties(old_ax, new_ax):
    # Copy lines from new_ax_1 to dm_ax0
    for line in old_ax.get_lines():
        new_ax.plot(line.get_xdata(), line.get_ydata(), label=line.get_label(), 
                    color=line.get_color(), linestyle=line.get_linestyle(), 
                    linewidth=line.get_linewidth())

    # Copy titles, labels, limits, and legend
    new_ax.set_title(old_ax.get_title())
    new_ax.set_xlabel(old_ax.get_xlabel())
    new_ax.set_ylabel(old_ax.get_ylabel())
    new_ax.set_xlim(old_ax.get_xlim())
    new_ax.set_ylim(old_ax.get_ylim())

    # Copy over the legend if it exists
    if old_ax.get_legend():
        new_ax.legend()


fs = 200  # Sampling frequency (16 kHz)
duration = 1  # Duration of the signal in seconds
time = np.linspace(0, duration, fs * duration, endpoint=False)  # Time vector

# Generate a random signal (simulating a speech signal)
signal = np.random.normal(0, 1, len(time))  # Random noise signal (mean 0, std 1)

plot = plt.figure(facecolor='none')
ax1 = plot.add_subplot(111, facecolor='none')
ax1.plot(signal)
ax1.axvline(x=100, color='red')
ax1.set_xlim(0, 120)
plt.show()


plot = plt.figure(facecolor='none')
ax2 = plot.add_subplot(111, facecolor='none')
ax2.plot(signal)
ax2.axvline(x=50, color='black')
plt.show()

print(ax1)

fig, (dm_ax0, dm_ax1) = plt.subplots(2, 1)

copy_axis_properties(ax1, dm_ax0)
copy_axis_properties(ax2, dm_ax1)


# old_fig = new_ax_1.figure
# new_ax_1.remove()
# new_ax_1.figure = fig
# fig.axes.append(new_ax_1)
# fig.add_subplot(new_ax_1)
# fig.add_axes(new_ax_1)

# def cb(artist):
#     print(artist)

# # dm_ax0.add_callback(cb)

# fig.add_callback(cb)
# # new_ax_1.add_callback() .set_position(dm_ax0)

# new_ax_2 = copy.deepcopy(ax2)
# old_fig = new_ax_2.figure
# new_ax_2.remove()
# new_ax_2.figure = fig
# fig.axes.append(new_ax_2)
# fig.add_subplot(new_ax_2)
# fig.add_axes(new_ax_2)

# new_ax_2.set_position(dm_ax1.get_position())

# new_ax_2 = plt.axes(ax2)
# fig.add_axes(new_ax_1)
# fig.add_axes(new_ax_2)

plt.show()