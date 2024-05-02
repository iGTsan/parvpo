class Lock {
    constructor() {
      this._locked = false;
      this._waiting = [];
    }
  
    lock() {
      const unlock = () => {
        let nextResolve;
        if (this._waiting.length > 0) {
          nextResolve = this._waiting.shift();
          nextResolve(unlock);
        } else {
          this._locked = false;
        }
      };
  
      if (this._locked) {
        return new Promise(resolve => {
          this._waiting.push(resolve);
        });
      } else {
        this._locked = true;
        return new Promise(resolve => resolve(unlock));
      }
    }
  }
  
  class SharedObject {
    constructor(data) {
      this.data = data;
      this.lock = new Lock();
    }
  
    async get() {
      const release = await this.lock.lock();
      return {data: this.data, release: release};
    }
  }

  module.exports = SharedObject;