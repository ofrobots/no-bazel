
import * as download from 'download';
import * as fs from 'fs';
import * as Listr from 'listr';
import * as makeDir from 'make-dir';
import * as meow from 'meow';
import * as path from 'path';
import * as tar from 'tar';

interface DependencyDescriptor {
  type: 'http-archive';
  url: string;
  location: string;
  sha256?: string;
  strip?: number;
}

interface Bag<T> {
  [name: string]: DependencyDescriptor;
}

const cli =
    meow({help: `Usage: ${process.execArgv} ${process.argv[1]} rootDirectory`});

if (cli.input.length < 1) {
  cli.showHelp(1);
}

run(cli.input[0]).catch(console.error);

async function run(rootDirectory: string) {
  try {
    await fs.promises.stat(rootDirectory);
  } catch (e) {
    throw new Error(`Unable to stat root directory ${rootDirectory}`);
  }

  const DEPS_STRING =
      await fs.promises.readFile(path.join(rootDirectory, 'DEPS.json'), 'utf8');
  const DEPS = JSON.parse(DEPS_STRING) as Bag<DependencyDescriptor>;
  // TODO(ofrobots): validate schema.

  const tasks = Object.keys(DEPS).map((name) => {
    const dep = DEPS[name];
    const targetDir = path.join(rootDirectory, dep.location);
    return {
      title: `Dependency ${name}`,
      task: () => {
        return new Listr([
          {
            title: `make directory ${targetDir}`,
            task: () => makeDir(targetDir)
          },
          {
            // TODO: separate into seperate download, verify sha and extract
            // steps.
            title: `download and extract`,
            task: () => {
              const s = download(dep.url).pipe(
                  tar.extract({cwd: targetDir, strip: dep.strip}));
              return new Promise((resolve, reject) => {
                s.on('end', resolve);
                s.on('error', reject);
              });
            }
          }
        ]);
      }
    };
  });

  const listrTasks = new Listr(tasks);
  await listrTasks.run();
}
