from math import log10

import matplotlib.pyplot as plt

# font = {'family': 'normal',
#         # 'weight': 'bold',
#         'size': 22}
# plt.rc('font', **font)

data = """
To 100000 sequentialSieveBasic time: 0.000255
To 100000 sequentialSieveBasic time: 0.000262
To 100000 sequentialSieveBasic time: 0.000101
To 100000 sequentialSieveBasic time: 0.000140
To 100000 sequentialSieveBasic time: 0.000259
To 100000 sequentialSieveBasic time: 0.000454
To 100000 sequentialSieveBasic time: 0.000257
To 100000 sequentialSieveBasic time: 0.000259
To 100000 sequentialSieveBasic time: 0.000257
To 100000 sequentialSieveBasic time: 0.000263
To 1000000 sequentialSieveBasic time: 0.002825
To 1000000 sequentialSieveBasic time: 0.002817
To 1000000 sequentialSieveBasic time: 0.002631
To 1000000 sequentialSieveBasic time: 0.001521
To 1000000 sequentialSieveBasic time: 0.002760
To 1000000 sequentialSieveBasic time: 0.002856
To 1000000 sequentialSieveBasic time: 0.002808
To 1000000 sequentialSieveBasic time: 0.002753
To 1000000 sequentialSieveBasic time: 0.002802
To 1000000 sequentialSieveBasic time: 0.002862
To 10000000 sequentialSieveBasic time: 0.047158
To 10000000 sequentialSieveBasic time: 0.051793
To 10000000 sequentialSieveBasic time: 0.050733
To 10000000 sequentialSieveBasic time: 0.048062
To 10000000 sequentialSieveBasic time: 0.044785
To 10000000 sequentialSieveBasic time: 0.046829
To 10000000 sequentialSieveBasic time: 0.042833
To 10000000 sequentialSieveBasic time: 0.043290
To 10000000 sequentialSieveBasic time: 0.047278
To 10000000 sequentialSieveBasic time: 0.043844
To 100000000 sequentialSieveBasic time: 0.824804
To 100000000 sequentialSieveBasic time: 0.827414
To 100000000 sequentialSieveBasic time: 0.822383
To 100000000 sequentialSieveBasic time: 0.821474
To 100000000 sequentialSieveBasic time: 0.832027
To 100000000 sequentialSieveBasic time: 0.821048
To 100000000 sequentialSieveBasic time: 0.819525
To 100000000 sequentialSieveBasic time: 0.829265
To 100000000 sequentialSieveBasic time: 0.828944
To 100000000 sequentialSieveBasic time: 0.827598
To 1000000000 sequentialSieveBasic time: 9.979650
To 1000000000 sequentialSieveBasic time: 10.157154
To 1000000000 sequentialSieveBasic time: 9.966333
To 1000000000 sequentialSieveBasic time: 9.930720
To 1000000000 sequentialSieveBasic time: 9.914427
To 1000000000 sequentialSieveBasic time: 9.916175
To 1000000000 sequentialSieveBasic time: 9.881673
To 1000000000 sequentialSieveBasic time: 9.909929
To 1000000000 sequentialSieveBasic time: 9.879258
To 1000000000 sequentialSieveBasic time: 9.924957
To 100000 sequentialSieveAdvanced time: 0.000129
To 100000 sequentialSieveAdvanced time: 0.000136
To 100000 sequentialSieveAdvanced time: 0.000136
To 100000 sequentialSieveAdvanced time: 0.000144
To 100000 sequentialSieveAdvanced time: 0.000075
To 100000 sequentialSieveAdvanced time: 0.000128
To 100000 sequentialSieveAdvanced time: 0.000075
To 100000 sequentialSieveAdvanced time: 0.000073
To 100000 sequentialSieveAdvanced time: 0.000141
To 100000 sequentialSieveAdvanced time: 0.000222
To 1000000 sequentialSieveAdvanced time: 0.000641
To 1000000 sequentialSieveAdvanced time: 0.001162
To 1000000 sequentialSieveAdvanced time: 0.000607
To 1000000 sequentialSieveAdvanced time: 0.001677
To 1000000 sequentialSieveAdvanced time: 0.001174
To 1000000 sequentialSieveAdvanced time: 0.001177
To 1000000 sequentialSieveAdvanced time: 0.001167
To 1000000 sequentialSieveAdvanced time: 0.001185
To 1000000 sequentialSieveAdvanced time: 0.001162
To 1000000 sequentialSieveAdvanced time: 0.000612
To 10000000 sequentialSieveAdvanced time: 0.007960
To 10000000 sequentialSieveAdvanced time: 0.008298
To 10000000 sequentialSieveAdvanced time: 0.010334
To 10000000 sequentialSieveAdvanced time: 0.011059
To 10000000 sequentialSieveAdvanced time: 0.010746
To 10000000 sequentialSieveAdvanced time: 0.009758
To 10000000 sequentialSieveAdvanced time: 0.009900
To 10000000 sequentialSieveAdvanced time: 0.007986
To 10000000 sequentialSieveAdvanced time: 0.008102
To 10000000 sequentialSieveAdvanced time: 0.008897
To 100000000 sequentialSieveAdvanced time: 0.392879
To 100000000 sequentialSieveAdvanced time: 0.388413
To 100000000 sequentialSieveAdvanced time: 0.394652
To 100000000 sequentialSieveAdvanced time: 0.389396
To 100000000 sequentialSieveAdvanced time: 0.390296
To 100000000 sequentialSieveAdvanced time: 0.394265
To 100000000 sequentialSieveAdvanced time: 0.390409
To 100000000 sequentialSieveAdvanced time: 0.394699
To 100000000 sequentialSieveAdvanced time: 0.390297
To 100000000 sequentialSieveAdvanced time: 0.389096
To 1000000000 sequentialSieveAdvanced time: 4.755121
To 1000000000 sequentialSieveAdvanced time: 4.757192
To 1000000000 sequentialSieveAdvanced time: 4.775208
To 1000000000 sequentialSieveAdvanced time: 4.746070
To 1000000000 sequentialSieveAdvanced time: 4.764867
To 1000000000 sequentialSieveAdvanced time: 4.768491
To 1000000000 sequentialSieveAdvanced time: 4.733905
To 1000000000 sequentialSieveAdvanced time: 4.753295
To 1000000000 sequentialSieveAdvanced time: 4.740213
To 1000000000 sequentialSieveAdvanced time: 4.741167
"""

lines = [line for line in data.split("\n") if len(line) > 0]
print(lines)
data = [(int(line.split(' ')[1]), float(line.split(' ')[4]), line.split(' ')[2]) for line in lines]

print('\n'.join([str(dat) for dat in data]))

values0 = [[dat[0], dat[1]] for dat in data if dat[2] == 'sequentialSieveBasic']
values1 = [[dat[0], dat[1]] for dat in data if dat[2] == 'sequentialSieveAdvanced']

def drawLine(values, label):
    x, y = zip(*values)
    # print(x)
    # print(y)

    xs = sorted([xv for xv in set(x)])
    yavg = []
    for x in xs:
        lst = [yv[1] for yv in values if yv[0] == x]

        yavg.append(sum(lst) / len(lst))

    # print(xs)
    # print(yavg)
    plt.plot(yavg, xs, label=label)
    # return xs, yavg


# figure = plt.figure(figsize=(16, 12))


drawLine(values0, "All values")
drawLine(values1, "Only odd values")

# plt.plot(yavg, xs)
plt.xlabel('time (s)')
plt.ylabel('N')
plt.legend()
# plt.yscale('log')
# plt.xscale('log')

plt.savefig('data.pdf', bbox_inches='tight')
plt.savefig('data.png', bbox_inches='tight')


def outputTable(values):
    xs0 = sorted(set([v[0] for v in values]))

    mins = []
    avgs = []
    maxs = []

    for x in xs0:
        lst = [yv[1] for yv in values if yv[0] == x]

        mins.append(min(lst))
        avgs.append(sum(lst) / len(lst))
        maxs.append(max(lst))


    def latexJoin(values, whole=False):
        val = ""
        first = True
        for v in values:
            if not first:
                val += ' & '
            first = False
            if whole:
                if log10(v) == int(log10(v)):
                    val += f'$10^{int(log10(v))}$'
                else:
                    val += f'${v}$'
            else:
                val += f'${v:5.5f}$'

        return val

    print('N & ', latexJoin(xs0, True), '\\\\ \\hline')
    print('min & ', latexJoin(mins), '\\\\ \\hline')
    print('avg & ', latexJoin(avgs), '\\\\ \\hline')
    print('max & ', latexJoin(maxs), '\\\\ \\hline')
    # print('min', '$&$'.join([str(x) for x in mins]))
    # print('avg', '$&$'.join([str(x) for x in avgs]))
    # print('max', '$&$'.join([str(x) for x in maxs]))


outputTable(values0)
outputTable(values1)

