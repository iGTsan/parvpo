const sqlite3 = require('sqlite3').verbose();
const fs = require("fs");
const log = require('./log');
const SharedObject = require('./shared_object');

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

module.exports.insert = async function (db, values) {
  const querry = `INSERT INTO user_activity (user_id, state, time) VALUES (?, ?, ?)`;
  // log(querry);
  // log(db);
  await db.run(querry, values, function(err) {
    if (err) {
      log(querry);
      log(values);
      return log("level=error", err.message);
    }
    // get the last insert id
    log(`level=info A row has been inserted with rowid ${this.lastID}`);
  });
  await sleep(20);
  return true;
}

function create_folder(path) {
  return new Promise(function(resolve, reject) {
    fs.access(path, fs.constants.F_OK, (err) => {
      if (err) {// Создаем папку
        fs.mkdir(path, { recursive: true }, (err) => {
          if (err) {
            resolve(false);
          } else {
            resolve(true);
          }
        });
      } else {
        resolve(false);
      }
    });
  });
}

async function run_sql_command(db, command) {
  return new Promise(function(resolve, reject) {
    // log(`Command: ${command} started`);
    const res = db.run(command, [], function(err) {
      // log(`Command: ${command} applied`);
      resolve(db);
    });
  });
}

async function run_sql_script(db, script) {
  log("level=info Running SQL script");
  const commands = script.split('\n');
  for (let i = 0; i < commands.length; i++) {
    db = await run_sql_command(db, commands[i]);
  }
  log("level=info SQL script ended");
  return new Promise(function(resolve, reject) {
    resolve(db);
  });
}

function fill_db(db, filename) {
  const filePath = `./sqlscripts/${filename}.sql`;
  return new Promise(function(resolve, reject) {
    fs.readFile(filePath, 'utf8', (err, text) => {
      if (err) {
        log(err, "level=error Error while opening file while filling DB");
        resolve(undefined);
      } else {
        log("level=info SQL file openned");
        run_sql_script(db, text).then(filled_db => {
          log("level=info Table filled");
          resolve(filled_db);
        });
      }
    });
  });
}

function create_db(filename) {
  return new Promise(function(resolve, reject) {
    let db = new sqlite3.Database(`./db/${filename}.db`, (err) => {
      if (err) {
        create_folder('./db').then(created => {
          if (created) {
            log("level=warning Another try to create DB");
            create_db(filename).then(res => {
              resolve(res);
            });
          } else {
            log("level=error Can't create DB in create_db");
            console.error("level=error", err);
            resolve(undefined);
          }
        });
      } else {
        log(`level=info DB created ${filename}.db`);
        fill_db(db, filename).then(filled_db => {
          log(`level=info Filled db ${filename}.db getted`);
          resolve(filled_db);
          log(`level=info Filled db ${filename}.db resolved`);
        });
      }
    });
  });
}

module.exports.open = async function (filename) {
  log(`level=info Start openning ${filename}.db`);
  return new Promise(function(resolve, reject) {
    let db = new sqlite3.Database(`./db/${filename}.db`, sqlite3.OPEN_READWRITE, (err) => {
      if (err) {
        create_db(filename).then(res => {
          if (res == undefined) {
            log("level=warning Can't create DB in open");
            console.error(err);
          }
          resolve(new SharedObject(res));
        });
      } else {
        log(`level=info Connected to the SQlite database ${filename}.db`);
        resolve(new SharedObject(db));
      }
    });
  });
}