import pandas as pd
import numpy as np
import math
import matplotlib.pyplot as plt
import matplotlib as mpl
import os


expname = "rob"
vers = ["gcc","perl"]

for version in vers:
    csvname = expname + "_" + version + ".csv"
    data = pd.read_csv(csvname)
    df = pd.DataFrame(data, columns=['ROB Size', 'Width', 'IQ Size', '#Instructions', '# Cycles', 'IPC'])
    print(df)


    dfP = df.pivot(index='ROB Size', columns='Width', values='IPC')


    print(dfP)

    # create valid markers from mpl.markers
    valid_markers = ([item[0] for item in mpl.markers.MarkerStyle.markers.items() if
                      item[1] is not 'nothing' and not item[1].startswith('tick') and not item[1].startswith('caret')])

    # valid_markers = mpl.markers.MarkerStyle.filled_markers
    # print(valid_markers)
    # markers = np.random.choice(valid_markers, df.shape[1], replace=False)

    labs = [32, 64, 128, 256, 512]

    df=dfP

    ax=df.plot(kind='line', markersize='10')
    for i, line in enumerate(ax.get_lines()):
        if i < 1:
            line.set_marker(valid_markers[i])
        else:
            line.set_marker(valid_markers[i+6])
        #if i is 2:
        #   line.set_linestyle('dashed')

    plt.xlabel("ROB Size")
    plt.ylabel("Instructions Per Cycle")
    plt.title("IPC vs. ROB Size for "+ version.upper() + " Trace")
    plt.legend(title="Proc. Width", loc='upper left')
    ax.set_xticks(labs)
    #ax.set_xticklabels(labs)
    #plt.show()
    fig = plt.gcf()
    fig.tight_layout()
    fig.set_size_inches(8, 6, forward=True)
    outFname = expname + "_" + version + ".png"
    fig.savefig(outFname)