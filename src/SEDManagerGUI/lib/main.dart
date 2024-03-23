import 'package:flutter/material.dart';
import 'interface/drive_launcher_page.dart';

void main() {
  runApp(const SEDManagerApp());
}

class SEDManagerApp extends StatelessWidget {
  const SEDManagerApp({super.key});

  ThemeData _getTheme() {
    //const accent = Color.fromARGB(255, 25, 145, 45);
    const accent = Color.fromARGB(255, 0, 156, 204);
    const background = Color.fromARGB(255, 24, 24, 24);
    const foreground = Color.fromARGB(255, 220, 220, 220);
    const errorTint = Color.fromARGB(255, 242, 16, 16);
    const inverse = Color.fromARGB(255, 36, 138, 255);

    final colorScheme = ColorScheme(
      background: background,
      brightness: Brightness.dark,
      error: Color.lerp(foreground, errorTint, 0.75)!,
      errorContainer: Color.lerp(background, errorTint, 0.25)!,
      inversePrimary: foreground,
      inverseSurface: inverse,
      onBackground: foreground,
      onError: foreground,
      onErrorContainer: foreground,
      onInverseSurface: foreground,
      onPrimary: Color.lerp(foreground, Colors.white, 0.5)!,
      onPrimaryContainer: Color.lerp(foreground, Colors.white, 0.5)!,
      onSecondary: Color.lerp(foreground, Colors.white, 0.5)!,
      onSecondaryContainer: Color.lerp(foreground, Colors.white, 0.5)!,
      onSurface: foreground,
      onSurfaceVariant: foreground,
      onTertiary: foreground,
      onTertiaryContainer: foreground,
      outline: Color.lerp(accent, background, 0.2)!,
      outlineVariant: Color.lerp(accent, background, 0.5)!,
      primary: accent,
      primaryContainer: Color.lerp(accent, background, 0.15)!,
      scrim: Color.lerp(accent, background, 0.15)!,
      secondary: Color.lerp(accent, background, 0.60)!,
      secondaryContainer: Color.lerp(accent, background, 0.68)!,
      shadow: Colors.black,
      surface: Color.lerp(foreground, background, 0.85)!,
      surfaceTint: accent,
      surfaceVariant: Color.lerp(foreground, background, 0.90)!,
      tertiary: Color.lerp(accent, background, 0.82)!,
      tertiaryContainer: Color.lerp(accent, background, 0.88)!,
    );

    return ThemeData(
      colorScheme: colorScheme,
      appBarTheme: AppBarTheme(
        backgroundColor: colorScheme.tertiary,
        foregroundColor: colorScheme.onSecondary,
      ),
      useMaterial3: true,
    );
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'SEDManager',
      theme: _getTheme(),
      home: DriveLauncherPage(() {}),
    );
  }
}
