def final(file, val, pref):
    max_data = max(map(max, val))
    min_data = min(map(min, val))
    data_color = [[int(255*(x-min_data)/(max_data-min_data)) for x in l] for l in val]
    with open(file+pref, 'wb') as fpng:
        w = png.Writer(len(val[0]), len(val), greyscale=True)
        w.write(fpng, data_color)
