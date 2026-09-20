import test from 'node:test';
import assert from 'node:assert/strict';
import { scrollProgress, ringFill, sceneAt, scenePalette, contrast } from '../web/scroll.js';

test('ring starts partially lit and closes only at the bottom of the page', () => {
  assert.equal(ringFill(scrollProgress(0, 5000, 1000)), 12);
  assert.equal(ringFill(scrollProgress(2000, 5000, 1000)), 56);
  assert.equal(ringFill(scrollProgress(4000, 5000, 1000)), 100);
});
test('overscroll, pages shorter than the viewport, and scroll reversal remain bounded', () => {
  assert.equal(scrollProgress(-50, 5000, 1000), 0);
  assert.equal(scrollProgress(9000, 5000, 1000), 1);
  assert.equal(scrollProgress(0, 700, 900), 0);
  const fills = [4000, 2000, 0].map(y => ringFill(scrollProgress(y, 5000, 1000)));
  assert.deepEqual(fills, [100, 56, 12]);
});
test('reduced motion keeps product fixed while preserving the informative fill', () => {
  for (const progress of [0, .25, .5, .75, 1]) {
    const scene = sceneAt(progress, true);
    assert.equal(scene.rotateX, 0); assert.equal(scene.rotateY, 0); assert.equal(scene.rotateZ, 0);
    assert.equal(scene.shiftX, 0); assert.equal(scene.shiftY, 0); assert.equal(scene.scale, 1);
    assert.equal(scene.fill, ringFill(progress));
  }
});

test('text remains readable throughout the light-to-dark transition', () => {
  for (let step = 0; step <= 100; step++) {
    const palette = scenePalette(step / 100);
    for (const key of ['ink', 'muted', 'accent']) assert.ok(contrast(palette[key], palette.bg) >= 4.5, `${key} failed at dusk ${step}`);
  }
});
