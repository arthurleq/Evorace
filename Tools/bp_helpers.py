import json
BT = 'editor_toolset.toolsets.blueprint.BlueprintTools.'
VT = 'EvoraceEditor.VersusEditorToolset.'
OT = 'editor_toolset.toolsets.object.ObjectTools.'
AT = 'editor_toolset.toolsets.asset.AssetTools.'
ACT = 'editor_toolset.toolsets.actor.ActorTools.'

def t(_tool, **kw):
    return execute_tool(_tool, json.dumps(kw))['returnValue']

def R(p):
    return {'refPath': p}

def BP(p):
    n = p.split('/')[-1]
    return {'refPath': p + '.' + n}

BLUE = {'r': 0.1, 'g': 0.3, 'b': 0.6, 'a': 0.55}
GREEN = {'r': 0.1, 'g': 0.45, 'b': 0.15, 'a': 0.55}
ORANGE = {'r': 0.6, 'g': 0.3, 'b': 0.05, 'a': 0.55}
PURPLE = {'r': 0.4, 'g': 0.15, 'b': 0.5, 'a': 0.55}
GREY = {'r': 0.3, 'g': 0.3, 'b': 0.3, 'a': 0.55}

def touch(bp):
    # Force le rafraichissement du cache de noeuds de ce Blueprint.
    t(BT + 'add_variable', blueprint=bp, name='ZZ_Tmp', type_name='bool')
    t(BT + 'remove_variable', blueprint=bp, name='ZZ_Tmp')

def fn(bp, name, ins=(), outs=(), obj_ins=(), obj_outs=(), tooltip=None):
    g = t(BT + 'add_function_graph', blueprint=bp, graph_name=name)
    for n, ty in ins:
        t(BT + 'add_function_param', graph=g, param_name=n, param_type=ty, input_param=True)
    for n, cls in obj_ins:
        t(BT + 'add_object_function_param', graph=g, param_name=n, object_class=R(cls), input_param=True)
    for n, ty in outs:
        t(BT + 'add_function_param', graph=g, param_name=n, param_type=ty, input_param=False)
    for n, cls in obj_outs:
        t(BT + 'add_object_function_param', graph=g, param_name=n, object_class=R(cls), input_param=False)
    if tooltip:
        t(VT + 'SetFunctionTooltip', FunctionGraph=g, Tooltip=tooltip)
    return g

def sizes(graph):
    d = {}
    for line in t(VT + 'DescribeNodes', Graph=graph):
        parts = [p.strip() for p in line.split(' | ')]
        x, y = [int(v) for v in parts[-2].split(',')]
        w, h = [float(v) for v in parts[-1].split('x')]
        d[parts[0]] = (x, y, w, h, parts[1])
    return d

def layout(graph, comments, default_color=None, font=16):
    """Range chaque evenement/fonction l'un sous l'autre et l'encadre d'un commentaire.
    comments: liste de (sous-chaine du type_id de l'entree, texte, couleur ou None)."""
    default_color = default_color or BLUE
    t(VT + 'RemoveAllComments', Graph=graph)
    entries = t(BT + 'find_nodes', graph=graph, title='', node_class=None, entry_points_only=True)
    infos = t(BT + 'get_node_infos', nodes=entries) if entries else []
    y = 0
    for e, info in zip(entries, infos):
        sub = [i['node'] for i in t(BT + 'get_connected_subgraph', node=e)] or [e]
        t(BT + 'arrange_nodes', nodes=sub)
        text, color = None, default_color
        for key, txt, col in comments:
            if key in info['type_id']:
                text, color = txt, (col or default_color)
                break
        nlines = (text.count('\n') + 1) if text else 0
        head = nlines * (font + 10) + 30
        sz = sizes(graph)
        names = [n['refPath'].split('.')[-1] for n in sub]
        minx = min(sz[n][0] for n in names)
        miny = min(sz[n][1] for n in names)
        maxx = max(sz[n][0] + sz[n][2] for n in names)
        maxy = max(sz[n][1] + sz[n][3] for n in names)
        dx = -minx
        dy = y + head + 40 - miny
        for n, nm in zip(sub, names):
            t(BT + 'set_node_position', node=n, pos={'x': int(sz[nm][0] + dx), 'y': int(sz[nm][1] + dy)})
        w = maxx - minx
        h = maxy - miny
        if text:
            t(VT + 'AddComment', Graph=graph, Text=text, X=-40, Y=y, Width=int(w + 80), Height=int(h + head + 80), Color=color, FontSize=font)
        y += int(h + head + 80) + 200
    return [i['type_id'] for i in infos]
