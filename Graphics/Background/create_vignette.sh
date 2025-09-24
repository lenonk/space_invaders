cat > vignette.svg <<'SVG'
<svg xmlns="http://www.w3.org/2000/svg" width="512" height="512">
  <defs>
    <radialGradient id="g" cx="50%" cy="50%" r="75%">
      <stop offset="60%" stop-color="black" stop-opacity="0"/>
      <stop offset="100%" stop-color="black" stop-opacity="1"/>
    </radialGradient>
  </defs>
  <rect width="100%" height="100%" fill="url(#g)"/>
</svg>
SVG
mkdir -p Graphics
rsvg-convert -w 512 -h 512 vignette.svg -o Graphics/vignette.png
