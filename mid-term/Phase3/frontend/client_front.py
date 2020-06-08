from flask import Flask, render_template, request, redirect, url_for
import os,signal
import subprocess
app = Flask(__name__)
app.config['SECRET_KEY'] = 'our very hard to guess secretfir'
process_pid = -1

# Simple form handling using raw HTML forms
@app.route('/endcall', methods=['GET', 'POST'])
def endcall():
    error = ""
    if request.method == 'POST':
        os.kill(process_pid,signal.SIGINT)
        exit()

    # Render the sign-up page
    return render_template('endcall.html', message=error)
@app.route('/', methods=['GET', 'POST'])
def call():
    error = ""
    if request.method == 'POST':
        # Form being submitted; grab data from form.
        username = request.form['username']
        IPPORT = request.form['IPPORT']
        ip= IPPORT.split(":")[0]
        port= IPPORT.split(":")[1]
        # Validate form data
        if len(username) == 0 or len(IPPORT) == 0:
            # Form data failed validation; try again
            error = "Please supply both first and last name"
        else:
            process = subprocess.Popen(['./build/client',ip,port,username,'grp1'])
            process_pid=process.pid
            return redirect(url_for('endcall'))

    # Render the sign-up page
    return render_template('index.html', message=error)


# Run the application
app.run()


