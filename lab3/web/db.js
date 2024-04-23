const sqlite3 = require('sqlite3').verbose();
const fs = require("fs");
const db_funcs = require('./db')
const shared = require('./shared_object');

module.exports.insert = async function (db, values) {
  const querry = `INSERT INTO user_activity (user_id, state, time) VALUES (?, ?, ?)`;
  await db.run(querry, values, function(err) {
    if (err) {
      console.log(querry);
      console.log(values);
      return console.log("Error", err.message);
    }
    // get the last insert id
    console.log(`A row has been inserted with rowid ${this.lastID}`);
  });
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
    // console.log(`Command: ${command} started`);
    const res = db.run(command, [], function(err) {
      // console.log(`Command: ${command} applied`);
      resolve(db);
    });
  });
}

async function run_sql_script(db, script) {
  console.log("Running SQL script");
  const commands = script.split('\n');
  for (let i = 0; i < commands.length; i++) {
    db = await run_sql_command(db, commands[i]);
  }
  console.log("SQL script ended");
  return new Promise(function(resolve, reject) {
    resolve(db);
  });
}

function fill_db(db, filename) {
  const filePath = `./sqlscripts/${filename}.sql`;
  return new Promise(function(resolve, reject) {
    fs.readFile(filePath, 'utf8', (err, text) => {
      if (err) {
        console.log(err, "Error while opening file while filling DB");
        resolve(undefined);
      } else {
        console.log("SQL file openned");
        run_sql_script(db, text).then(filled_db => {
          console.log("Table filled");
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
            console.log("Another try to create DB");
            create_db(filename).then(res => {
              resolve(res);
            });
          } else {
            console.log("Can't create DB in create_db");
            console.error(err);
            resolve(undefined);
          }
        });
      } else {
        console.log(`DB created ${filename}.db`);
        fill_db(db, filename).then(filled_db => {
          console.log(`Filled db ${filename}.db getted`);
          resolve(filled_db);
          console.log(`Filled db ${filename}.db resolved`);
        });
      }
    });
  });
}

function sleep(ms) {
  return new Promise((resolve) => {
    setTimeout(resolve, ms);
  });
}

module.exports.open = async function (filename) {
  console.log(`start openning ${filename}.db`);
  return new Promise(function(resolve, reject) {
    let db = new sqlite3.Database(`./db/${filename}.db`, sqlite3.OPEN_READWRITE, (err) => {
      if (err) {
        create_db(filename).then(res => {
          if (res == undefined) {
            console.log("Can't create DB in open");
            console.error(err);
          }
          resolve(res);
        });
      } else {
        console.log(`Connected to the SQlite database ${filename}.db`);
        resolve(new SharedObject(db));
      }
    });
  });
}

async function get_rows(db, sql) {
  return new Promise(function(resolve, reject) {
    // console.log(sql);
    db.all(sql, [], (err, rows) => {
      if (err) {
        throw err;
      }
      resolve(rows);
    });
  });
}

module.exports.gen_rating_table = async function (db) {
  const sql = `SELECT users.user_id as ID,
                users.division as DIVISION,
                (criterion_1.grade_1 + criterion_1.grade_2 + criterion_1.grade_3) as GRADE_1,
                (criterion_2.grade_1 + criterion_2.grade_2) as GRADE_2,
                (criterion_3.grade_1 + criterion_3.grade_2 + criterion_3.grade_3 + criterion_3.grade_4) as GRADE_3,
                (criterion_4.grade_1 + criterion_4.grade_2) as GRADE_4,
                (criterion_5.grade_1 + criterion_5.grade_2 + criterion_5.grade_3 + criterion_5.grade_4 + criterion_5.grade_5) as GRADE_5,
                (criterion_1.grade_1 + criterion_1.grade_2 + criterion_1.grade_3 +
                criterion_2.grade_1 + criterion_2.grade_2 +
                criterion_3.grade_1 + criterion_3.grade_2 + criterion_3.grade_3 + criterion_3.grade_4 +
                criterion_4.grade_1 + criterion_4.grade_2 +
                criterion_5.grade_1 + criterion_5.grade_2 + criterion_5.grade_3 + criterion_5.grade_4 + criterion_5.grade_5) as TOTAL_GRADE
                FROM users
                INNER JOIN criterion_1
                ON users.user_id = criterion_1.user_id
                INNER JOIN criterion_2
                ON users.user_id = criterion_2.user_id
                INNER JOIN criterion_3
                ON users.user_id = criterion_3.user_id
                INNER JOIN criterion_4
                ON users.user_id = criterion_4.user_id
                INNER JOIN criterion_5
                ON users.user_id = criterion_5.user_id
                ORDER BY (criterion_1.grade_1 + criterion_1.grade_2 + criterion_1.grade_3 +
                criterion_2.grade_1 + criterion_2.grade_2 +
                criterion_3.grade_1 + criterion_3.grade_2 + criterion_3.grade_3 + criterion_3.grade_4 +
                criterion_4.grade_1 + criterion_4.grade_2 +
                criterion_5.grade_1 + criterion_5.grade_2 + criterion_5.grade_3 + criterion_5.grade_4 + criterion_5.grade_5) DESC`;
  return get_rows(db, sql);
}
