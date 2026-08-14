import fs from "node:fs";
import path from "node:path";

const root = process.cwd();
const buildDir = path.join(root, "firmware", "build");
const inputPath = path.join(buildDir, "compile_commands.json");
const outputPath = path.join(buildDir, "compile_commands.json");
const sketchDir = path.join(root, "firmware");
const sketchSource = path.join(sketchDir, "firmware.ino");

if (!fs.existsSync(inputPath)) {
  console.error(`Input compile_commands.json not found at ${inputPath}`);
  process.exit(1);
}

const db = JSON.parse(fs.readFileSync(inputPath, "utf8"));
const generatedSketch = path.join(
  buildDir,
  "sketch",
  "firmware.ino.cpp"
);

const sketchEntry = db.find((entry) => entry.file === generatedSketch);

if (!sketchEntry) {
  console.error(`Unable to find generated sketch entry for: ${generatedSketch}`);
  process.exit(1);
}

// Helper to check if file is source or header
function getLanguage(ext) {
  if (ext === ".h" || ext === ".hpp") return "c++-header";
  return "c++";
}

function entryForSource(sourceFile) {
  const ext = path.extname(sourceFile);
  const language = getLanguage(ext);
  
  // Clone arguments
  const args = [...sketchEntry.arguments];
  
  // Replace the generated sketch file in arguments with our source file
  const sourceIndex = args.indexOf(generatedSketch);
  if (sourceIndex !== -1) {
    args.splice(sourceIndex, 1, "-x", language, sourceFile);
  } else {
    // If not found in template, we can just replace the last argument before -o
    const outputIndex = args.indexOf("-o");
    if (outputIndex > 0) {
      args.splice(outputIndex - 1, 1, "-x", language, sourceFile);
    }
  }

  // Adjust output file path
  const outputIndex = args.indexOf("-o");
  if (outputIndex !== -1 && outputIndex + 1 < args.length) {
    const outputName = `${path.basename(sourceFile)}.o`;
    args[outputIndex + 1] = path.join(buildDir, "sketch", outputName);
  }

  return {
    directory: sketchDir,
    arguments: args,
    file: sourceFile,
  };
}

// Find all source/header files recursively in sketchDir
function getSketchFiles(dir) {
  let results = [];
  const list = fs.readdirSync(dir);
  list.forEach((file) => {
    const filePath = path.join(dir, file);
    const stat = fs.statSync(filePath);
    if (stat && stat.isDirectory()) {
      if (file !== "build" && file !== ".git" && file !== "node_modules") {
        results = results.concat(getSketchFiles(filePath));
      }
    } else {
      const ext = path.extname(file);
      if (ext === ".h" || ext === ".hpp" || ext === ".cpp" || ext === ".c" || ext === ".ino") {
        results.push(filePath);
      }
    }
  });
  return results;
}

const localFiles = getSketchFiles(sketchDir);
console.log("Found local files for compiler DB mapping:", localFiles);

const augmentedFiles = new Set(localFiles);
// Filter out entries in database that refer to build-sketch copies of our local files
const filteredDb = db.filter((entry) => {
  // If the file path in entry is inside the build/sketch folder, and matches one of our local files
  const baseName = path.basename(entry.file);
  const isSketchCopy = entry.file.includes(path.join(buildDir, "sketch")) && 
                       localFiles.some((f) => path.basename(f) === baseName || path.basename(f) + ".cpp" === baseName);
  return !isSketchCopy;
});

const augmentedDb = [
  ...filteredDb,
  ...localFiles.map((file) => entryForSource(file)),
];

fs.writeFileSync(outputPath, `${JSON.stringify(augmentedDb, null, 2)}\n`);
console.log(`Wrote ${outputPath} with ${augmentedDb.length} entries`);
