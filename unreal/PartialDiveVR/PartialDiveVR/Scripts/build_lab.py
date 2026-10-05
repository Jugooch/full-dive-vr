"""Build (or rebuild) the V0 lab level: /Game/PartialDive/Levels/L_Lab.

The level is GENERATED: edit this script, not the level, so the lab stays reproducible across sessions.
Run headless:
  UnrealEditor-Cmd.exe <PartialDiveVR.uproject> -ExecutePythonScript="<repo>/unreal/PartialDiveVR/PartialDiveVR/Scripts/build_lab.py" -unattended -nullrhi
or in the editor: Tools > Execute Python Script...

Layout (research guide Phase 1: "one tiny laboratory, not a game"): room, mirror, table, cube, sphere,
sword, target dummy. Player starts facing the mirror (+X).
"""
import unreal

LEVEL = "/Game/PartialDive/Levels/L_Lab"
MAT_DIR = "/Game/PartialDive/Materials"
MIRROR_MAT = MAT_DIR + "/M_PDMirror"
GRID = "/Game/LevelPrototyping/Materials/MI_PrototypeGrid_Gray"
GRID_DARK = "/Game/LevelPrototyping/Materials/MI_PrototypeGrid_TopDark"
CUBE = "/Engine/BasicShapes/Cube"

eal = unreal.EditorAssetLibrary
eas = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
les = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
mel = unreal.MaterialEditingLibrary


def log(msg):
    unreal.log("[build_lab] " + msg)


def ensure_mirror_material(rebuild=False):
    """Unlit material showing the mirror's scene capture, flipped horizontally, at screen-space UVs."""
    if eal.does_asset_exist(MIRROR_MAT):
        if not rebuild:
            return
        mat = unreal.load_asset(MIRROR_MAT)  # rebuild in place (it may be in use by the loaded level)
        mel.delete_all_material_expressions(mat)
    else:
        tools = unreal.AssetToolsHelpers.get_asset_tools()
        mat = tools.create_asset("M_PDMirror", MAT_DIR, unreal.Material, unreal.MaterialFactoryNew())
    mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)

    # uv = ViewportUV * (-1, 1) + (1, 0)  -> horizontal flip (a reflection inverts handedness)
    screen = mel.create_material_expression(mat, unreal.MaterialExpressionScreenPosition, -900, 0)
    scale = mel.create_material_expression(mat, unreal.MaterialExpressionConstant2Vector, -900, 120)
    scale.set_editor_property("r", -1.0)
    scale.set_editor_property("g", 1.0)
    offset = mel.create_material_expression(mat, unreal.MaterialExpressionConstant2Vector, -700, 120)
    offset.set_editor_property("r", 1.0)
    offset.set_editor_property("g", 0.0)
    mul = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, -700, 0)
    add = mel.create_material_expression(mat, unreal.MaterialExpressionAdd, -500, 0)
    tex = mel.create_material_expression(mat, unreal.MaterialExpressionTextureSampleParameter2D, -300, 0)
    tex.set_editor_property("parameter_name", "Capture")
    tex.set_editor_property("texture", unreal.load_asset("/Engine/EngineResources/Black"))

    links = [
        (screen, "ViewportUV", mul, "A"), (scale, "", mul, "B"),
        (mul, "", add, "A"), (offset, "", add, "B"),
        (add, "", tex, "UVs"),
    ]
    for src, out, dst, inp in links:
        if not mel.connect_material_expressions(src, out, dst, inp):
            raise RuntimeError("material link failed: %s.%s -> %s.%s" % (src.get_name(), out, dst.get_name(), inp))
    mel.connect_material_property(tex, "RGB", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    mel.recompile_material(mat)
    eal.save_asset(MIRROR_MAT)
    log("created " + MIRROR_MAT)


def box(label, loc, scale, material):
    a = eas.spawn_actor_from_object(unreal.load_asset(CUBE), unreal.Vector(*loc))
    a.set_actor_label(label)
    a.set_actor_scale3d(unreal.Vector(*scale))
    mesh = a.static_mesh_component
    mesh.set_material(0, unreal.load_asset(material))
    mesh.set_mobility(unreal.ComponentMobility.STATIC)
    return a


def spawn(cls, label, loc, yaw=0.0):
    a = eas.spawn_actor_from_class(cls, unreal.Vector(*loc), unreal.Rotator(roll=0.0, pitch=0.0, yaw=yaw))
    a.set_actor_label(label)
    return a


def prop(kind, label, loc, color, yaw=0.0):
    a = spawn(unreal.PDProp, label, loc, yaw)
    a.set_editor_property("color", unreal.LinearColor(*color, 1.0))
    a.set_editor_property("kind", kind)  # PostEditChange reruns the construction script
    return a


def build():
    ensure_mirror_material(rebuild=True)
    if eal.does_asset_exist(LEVEL):
        # Rebuild in place (the lab may be the loaded startup map, so it can't be deleted).
        if not les.load_level(LEVEL):
            raise RuntimeError("could not load " + LEVEL)
        for actor in eas.get_all_level_actors():
            if not isinstance(actor, (unreal.WorldSettings, unreal.Brush)):
                eas.destroy_actor(actor)
    elif not les.new_level(LEVEL):
        raise RuntimeError("could not create " + LEVEL)

    # Room: 8 m x 8 m, 3.5 m high. Floor/walls are WorldStatic (never trigger touch haptics).
    box("Floor", (0, 0, -5), (8, 8, 0.1), GRID)
    box("Wall_Front", (405, 0, 175), (0.1, 8, 3.5), GRID_DARK)
    box("Wall_Back", (-405, 0, 175), (0.1, 8, 3.5), GRID_DARK)
    box("Wall_Left", (0, -405, 175), (8, 0.1, 3.5), GRID_DARK)
    box("Wall_Right", (0, 405, 175), (8, 0.1, 3.5), GRID_DARK)

    sun = spawn(unreal.DirectionalLight, "Sun", (0, 0, 400))
    sun.set_actor_rotation(unreal.Rotator(roll=0.0, pitch=-50.0, yaw=35.0), False)
    sun.light_component.set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)
    sun.light_component.set_editor_property("atmosphere_sun_light", True)
    sky = spawn(unreal.SkyLight, "SkyLight", (0, 0, 300))
    sky.light_component.set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)
    sky.light_component.set_editor_property("real_time_capture", True)
    spawn(unreal.SkyAtmosphere, "SkyAtmosphere", (0, 0, 0))

    # Player starts 2.2 m from the mirror, facing it.
    spawn(unreal.PlayerStart, "PlayerStart", (-20, 0, 100))
    spawn(unreal.PDMirror, "Mirror", (200, 0, 0), yaw=180.0)

    prop(unreal.PDPropKind.TABLE, "Table", (60, -95, 75), (0.35, 0.25, 0.18))
    prop(unreal.PDPropKind.CUBE, "Cube", (45, -80, 90), (0.85, 0.3, 0.1))
    prop(unreal.PDPropKind.SPHERE, "Sphere", (70, -110, 90), (0.1, 0.45, 0.85))
    # Sword lying flat on the table: blade (local Z) along +Y, guard (local Y) along -X, blade flat side up.
    sword = prop(unreal.PDPropKind.SWORD, "Sword", (85, -150, 80), (0.8, 0.8, 0.8))
    sword.set_actor_rotation(unreal.MathLibrary.make_rot_from_zy(unreal.Vector(0, 1, 0), unreal.Vector(-1, 0, 0)), False)
    prop(unreal.PDPropKind.TARGET_DUMMY, "TargetDummy", (150, 170, 80), (0.6, 0.45, 0.3))

    world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
    world.get_world_settings().set_editor_property("default_game_mode", unreal.PDGameMode)

    les.save_current_level()
    log("saved " + LEVEL)


build()
