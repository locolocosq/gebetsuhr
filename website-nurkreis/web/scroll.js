export const clamp = (value, min = 0, max = 1) => Math.min(max, Math.max(min, value));

export function scrollProgress(scrollY, documentHeight, viewportHeight) {
  const distance = documentHeight - viewportHeight;
  return distance > 0 ? clamp(scrollY / distance) : 0;
}

export function ringFill(progress) {
  return 12 + 88 * clamp(progress);
}

export const mixRgb = (a, b, t) => a.map((n, i) => Math.round(n + (b[i] - n) * t));

function luminance(rgb) {
  const linear = rgb.map(value => { const v = value / 255; return v <= .04045 ? v / 12.92 : ((v + .055) / 1.055) ** 2.4; });
  return linear[0] * .2126 + linear[1] * .7152 + linear[2] * .0722;
}

export function contrast(a, b) {
  const x = luminance(a), y = luminance(b);
  return (Math.max(x, y) + .05) / (Math.min(x, y) + .05);
}

export function scenePalette(dusk) {
  const bg = mixRgb([237, 233, 225], [29, 31, 26], clamp(dusk));
  const dark = luminance(bg) < .179;
  const fallback = dark ? [255, 255, 255] : [0, 0, 0];
  const readable = candidate => contrast(candidate, bg) >= 4.5 ? candidate : fallback;
  return {
    bg,
    ink: readable(dark ? [242, 234, 219] : [41, 43, 37]),
    muted: readable(dark ? [182, 179, 165] : [102, 103, 94]),
    accent: readable(dark ? [234, 161, 101] : [170, 75, 37]),
  };
}

export function sceneAt(progress, reducedMotion = false) {
  const p = clamp(progress);
  const dusk = clamp((p - 0.38) / 0.35);
  return {
    fill: ringFill(p),
    dusk,
    rotateY: reducedMotion ? 0 : -22 + Math.sin(p * Math.PI * 1.5) * 29,
    rotateX: reducedMotion ? 0 : 7 - Math.sin(p * Math.PI) * 11,
    rotateZ: reducedMotion ? 0 : -7 + 12 * p,
    shiftX: reducedMotion ? 0 : Math.sin(p * Math.PI * 2) * 3,
    shiftY: reducedMotion ? 0 : -Math.sin(p * Math.PI) * 3,
    scale: reducedMotion ? 1 : 1 + Math.sin(p * Math.PI) * 0.1,
  };
}
