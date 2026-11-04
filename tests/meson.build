# Tests for Gyrus Application

test_sources = [
  'AssetManagerTest.cpp',
  'ConsoleLoggerTest.cpp',
  'FileReaderTest.cpp',
  'SoundEffectsTest.cpp',
  'FixedStepTest.cpp',
  'MusicTest.cpp',
  'AudioTest.cpp',
  'SceneGraphicsTest.cpp',
  'StarfieldTest.cpp',
  'EnemyShotsTest.cpp',
  'GameSessionTest.cpp',
  'EffectsTest.cpp',
  'UpgradeTest.cpp',
  'GraphicsTest.cpp',
  'ScoreTest.cpp',
  'TurnsTest.cpp',
  'CollisionTest.cpp',
]

foreach test_source : test_sources
  test_name = test_source.split('.')[0]
  
  test_exe = executable(test_name,
    test_source,
    include_directories: [inc_dirs, src_inc_dirs],
    dependencies: [lib_dep, gtest_dep, gtest_main_dep],
  )
  
  test(test_name, test_exe)
endforeach

polar_test = executable('PolarTest', 'PolarTest.cpp',
  include_directories: [inc_dirs, src_inc_dirs],
  dependencies: [lib_dep],
)
test('PolarTest', polar_test)

player_test = executable('PlayerTest', 'PlayerTest.cpp',
  include_directories: [inc_dirs, src_inc_dirs],
  dependencies: [lib_dep],
)
test('PlayerTest', player_test)

enemy_test = executable('EnemiesTest', 'EnemiesTest.cpp',
  include_directories: [inc_dirs, src_inc_dirs],
  dependencies: [lib_dep],
)
test('EnemiesTest', enemy_test)

formation_test = executable('FormationTest', 'FormationTest.cpp',
  include_directories: [inc_dirs, src_inc_dirs],
  dependencies: [lib_dep],
)
test('FormationTest', formation_test)

foreach module : ['Stage', 'Hazards']
  module_test = executable(module + 'Test', module + 'Test.cpp',
    include_directories: [inc_dirs, src_inc_dirs], dependencies: [lib_dep])
  test(module + 'Test', module_test)
endforeach
