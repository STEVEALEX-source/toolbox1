# Toolbox

I kept needing the same small tools and got annoyed jumping between apps. This is a set of pages with the ones I use most.

Timer, tasks, notes, password, converter, customs guess, color, calculator, random pick, habits, and waiting on.

Each tool has its own page. The home page is just the list.

Everything stays in the browser. No account. Tasks, notes, habits, waiting list and the theme preference use local storage.

## Run it

Needs Node.

```bash
node server.js
```

Then open http://localhost:8000

Different port if you want:

```bash
PORT=3000 node server.js
```

`npm start` works too.

## Files

index.html is the home list.
timer.html, tasks.html, notes.html, and the rest are the tools.
style.css is the look.
script.js is shared logic.
404.html is the missing page.
server.js is a tiny Node server.
package.json has the start script.

## Note

Customs is a ballpark figure. Do not treat it as real tax advice.
