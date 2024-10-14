import matplotlib.lines as mlines
import matplotlib.collections as mcollections
import matplotlib.patches as mpatches

def copy_axes(old_ax, new_ax):
    # Copy lines
    for line in old_ax.get_lines():
        new_line = mlines.Line2D(
            line.get_xdata(),
            line.get_ydata(),
            label=line.get_label(),
            color=line.get_color(),
            linestyle=line.get_linestyle(),
            linewidth=line.get_linewidth(),
            marker=line.get_marker(),
            markerfacecolor=line.get_markerfacecolor(),
            markersize=line.get_markersize(),
        )
        new_ax.add_line(new_line)

    # Copy collections (e.g., scatter plots, quad meshes)
    for collection in old_ax.collections:
        if isinstance(collection, mcollections.QuadMesh):
            new_collection = mcollections.QuadMesh(
                collection.get_coordinates(),
                antialiased=collection.get_antialiased(),
                facecolor=collection.get_facecolor(),
                edgecolor=collection.get_edgecolor(),
                linewidths=collection.get_linewidths()
            )
            new_ax.add_collection(new_collection)
        
        # elif isinstance(collection, mcollections.PathCollection):
        #     new_collection = mcollections.PathCollection(
        #         paths=collection.get_paths(),
        #         sizes=collection.get_sizes(),
        #         facecolor=collection.get_facecolor(),  # Get point colors
        #         edgecolor=collection.get_edgecolor(),  # Get edge colors if needed
        #         marker=collection.get_marker(),
        #     )
        #     new_ax.add_collection(new_collection)
        # print(collection)
        # if isinstance(collection, mcollections.PathCollection):
        #     new_collection = mcollections.PathCollection(
        #         collection.get_paths(),
        #         facecolor=collection.get_facecolor(),
        #         edgecolor=collection.get_edgecolor(),
        #         linewidths=collection.get_linewidths()
        #     )
        #     new_ax.add_collection(new_collection)

        # elif isinstance(collection, mcollections.QuadMesh):
        #     new_collection = mcollections.QuadMesh(
        #         collection.get_array(),
        #         facecolor=collection.get_facecolor(),
        #         edgecolor=collection.get_edgecolor(),
        #         linewidths=collection.get_linewidths()
        #     )
        #     new_ax.add_collection(new_collection)

    # Copy patches (e.g., rectangles, circles, polygons)
    for patch in old_ax.patches:
        if isinstance(patch, mpatches.Rectangle):
            new_patch = mpatches.Rectangle(
                patch.get_xy(),
                patch.get_width(),
                patch.get_height(),
                angle=patch.get_angle(),
                facecolor=patch.get_facecolor(),
                edgecolor=patch.get_edgecolor(),
                linewidth=patch.get_linewidth()
            )
        elif isinstance(patch, mpatches.Polygon):
            new_patch = mpatches.Polygon(
                patch.get_xy(),
                closed=True,
                facecolor=patch.get_facecolor(),
                edgecolor=patch.get_edgecolor(),
                linewidth=patch.get_linewidth()
            )
        
        new_ax.add_patch(new_patch)

    # Copy titles, labels, limits, and legend
    new_ax.set_title(old_ax.get_title())
    new_ax.set_xlabel(old_ax.get_xlabel())
    new_ax.set_ylabel(old_ax.get_ylabel())
    new_ax.set_xlim(old_ax.get_xlim())
    new_ax.set_ylim(old_ax.get_ylim())

    # Copy over the legend if it exists
    if old_ax.get_legend():
        new_ax.legend()

def flatten_2d(l):
    return [e for row in l for e in row]