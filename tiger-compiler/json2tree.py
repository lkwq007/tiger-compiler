from __future__ import print_function
import json
import sys

with open("tempout.json", "r") as f:
    s = f.readlines()
s = ''.join(s).replace(' ', '').replace('\n', '')
# Tree in JSON format
# s = '{"Harry": {"children": ["Bill", {"Jane": {"children": [{"Diane": {"children": ["Mary"]}}, "Mark"]}}]}}'

# Convert JSON tree to a Python dict
data = json.loads(s)

# Convert back to JSON & print to stderr so we can verfiy that the tree is correct.
# print(json.dumps(data, indent=4), file=sys.stderr)

# Extract tree edges from the dict
edges = []
i = 0
def get_edges(treedict, parent=None):
    global i
    for name in (treedict.keys()):
        # print(name)
        label = name
        label+=str(i)
        i+=1
        if parent is not None:
            edges.append((parent, label))
        if isinstance(treedict[name], dict):
            get_edges(treedict[name], parent=label)
            # for item in treedict[name].keys():
                # print(item)
        else:
            edges.append((label, treedict[name]))

get_edges(data)

# Dump edge list in Graphviz DOT format
print('strict digraph tree {')
for row in edges:
    print('    {0} -> {1};'.format(*row))
print('}')