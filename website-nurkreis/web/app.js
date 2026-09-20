import { clamp, scrollProgress, sceneAt, scenePalette } from './scroll.js';

const root = document.documentElement;
const rings = [...document.querySelectorAll('.ring-fill')];
const sections = [...document.querySelectorAll('[data-chapter]')];
const links = [...document.querySelectorAll('.chapter-nav a')];
const motionButton = document.querySelector('#motion-toggle');
const media = matchMedia('(prefers-reduced-motion: reduce)');
const dialog = document.querySelector('#project-dialog');
let reduced = media.matches;
let scheduled = false;
let documentHeight = 0;
let chapterPositions = [];
let previousFocus = null;

function measure() {
  documentHeight = document.documentElement.scrollHeight;
  chapterPositions = sections.map(section => section.offsetTop);
  requestUpdate();
}

function render() {
  scheduled = false;
  const progress = scrollProgress(scrollY, documentHeight, innerHeight);
  const state = sceneAt(progress, reduced);
  root.style.setProperty('--progress', progress);
  root.style.setProperty('--fill', state.fill);
  root.style.setProperty('--dusk', state.dusk);
  const palette = scenePalette(state.dusk);
  for (const [name, color] of Object.entries(palette)) root.style.setProperty(`--${name}`, `rgb(${color.join(' ')})`);
  root.style.setProperty('--line', `rgba(${state.dusk > .5 ? '228,220,201' : '55,54,43'},.19)`);
  root.style.setProperty('--rotate-x', `${state.rotateX}deg`);
  root.style.setProperty('--rotate-y', `${state.rotateY}deg`);
  root.style.setProperty('--rotate-z', `${state.rotateZ}deg`);
  root.style.setProperty('--shift-x', `${state.shiftX}%`);
  root.style.setProperty('--shift-y', `${state.shiftY}%`);
  root.style.setProperty('--scale', state.scale);
  for (const ring of rings) ring.setAttribute('stroke-dasharray', `${state.fill} 100`);
  let current = 0;
  chapterPositions.forEach((position, index) => { if (scrollY + innerHeight * .42 >= position) current = index; });
  if (progress >= .999) current = sections.length - 1;
  links.forEach((link, index) => {
    if (index === current) link.setAttribute('aria-current', 'step');
    else link.removeAttribute('aria-current');
  });
}

function requestUpdate() {
  if (!scheduled) { scheduled = true; requestAnimationFrame(render); }
}

function updateMotion() {
  root.dataset.reducedMotion = String(reduced);
  motionButton.setAttribute('aria-pressed', String(reduced));
  motionButton.textContent = reduced ? 'Bewegung aktivieren' : 'Bewegung reduzieren';
  requestUpdate();
}

motionButton.addEventListener('click', () => { reduced = !reduced; updateMotion(); });
media.addEventListener('change', event => { reduced = event.matches; updateMotion(); });
addEventListener('scroll', requestUpdate, { passive: true });
addEventListener('resize', measure, { passive: true });
addEventListener('pageshow', measure);
new ResizeObserver(measure).observe(document.querySelector('main'));

document.querySelectorAll('a[href^="#"]').forEach(link => {
  link.addEventListener('click', event => {
    const target = document.querySelector(link.getAttribute('href'));
    if (!target) return;
    event.preventDefault();
    const y = target.id === 'isha' ? documentHeight - innerHeight : target.offsetTop;
    scrollTo({ top: clamp(y, 0, documentHeight - innerHeight), behavior: reduced ? 'instant' : 'smooth' });
    history.replaceState(null, '', link.getAttribute('href'));
    if (link.classList.contains('skip-link')) { target.tabIndex = -1; target.focus({ preventScroll: true }); }
  });
});

document.querySelectorAll('[data-open-details]').forEach(button => button.addEventListener('click', () => {
  previousFocus = button;
  dialog.showModal();
  document.body.style.overflow = 'hidden';
}));
dialog.querySelectorAll('.close-button, .dialog-return').forEach(button => button.addEventListener('click', () => dialog.close()));
dialog.addEventListener('click', event => {
  const bounds = dialog.getBoundingClientRect();
  if (event.target === dialog && (event.clientX < bounds.left || event.clientX > bounds.right || event.clientY < bounds.top || event.clientY > bounds.bottom)) dialog.close();
});
dialog.addEventListener('close', () => { document.body.style.overflow = ''; previousFocus?.focus({ preventScroll: true }); measure(); });

measure();
updateMotion();
