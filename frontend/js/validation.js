/**
 * Form Validation Logic
 */

document.addEventListener('DOMContentLoaded', () => {
    
    // Form submission handlers
    const loginForm = document.getElementById('loginForm');
    if (loginForm) {
        loginForm.addEventListener('submit', (e) => {
            e.preventDefault();
            if (validateForm(loginForm)) {
                submitFormToCGI(loginForm, '/cgi-bin/login.cgi', (data) => {
                    if (data.status === 'success') {
                        window.App.showToast(data.message, 'success');
                        setTimeout(() => { window.location.href = 'dashboard.html'; }, 1500);
                    } else {
                        window.App.showToast(data.message, 'error');
                    }
                });
            }
        });
    }

    const registerForm = document.getElementById('registerForm');
    if (registerForm) {
        // Password strength meter
        const pwdInput = document.getElementById('reg_password');
        const strengthBar = document.getElementById('strengthBar');
        
        if (pwdInput && strengthBar) {
            pwdInput.addEventListener('input', () => {
                const val = pwdInput.value;
                strengthBar.className = 'strength-bar'; // reset
                
                if (val.length === 0) {
                    strengthBar.style.width = '0%';
                } else if (val.length < 6) {
                    strengthBar.classList.add('strength-weak');
                } else if (val.length < 10 && val.match(/[a-zA-Z]/) && val.match(/[0-9]/)) {
                    strengthBar.classList.add('strength-medium');
                } else if (val.length >= 10 && val.match(/[A-Z]/) && val.match(/[0-9]/) && val.match(/[^a-zA-Z0-9]/)) {
                    strengthBar.classList.add('strength-strong');
                } else {
                    strengthBar.classList.add('strength-medium'); // default decent
                }
            });
        }

        registerForm.addEventListener('submit', (e) => {
            e.preventDefault();
            if (validateForm(registerForm)) {
                const pwd = document.getElementById('reg_password').value;
                const confirm = document.getElementById('reg_confirm_password').value;
                if (pwd !== confirm) {
                    showError(document.getElementById('reg_confirm_password'), 'Passwords do not match');
                    return;
                }

                submitFormToCGI(registerForm, '/cgi-bin/register.cgi', (data) => {
                    if (data.status === 'success') {
                        window.App.showToast(data.message + ' Account No: ' + data.account_number, 'success');
                        setTimeout(() => { window.location.href = 'login.html'; }, 3000);
                    } else {
                        window.App.showToast(data.message, 'error');
                    }
                });
            }
        });
    }

    // Generic form actions in dashboard (Deposit, Withdraw, Transfer)
    const depositForm = document.getElementById('depositForm');
    if (depositForm) {
        depositForm.addEventListener('submit', (e) => {
            e.preventDefault();
            if (validateForm(depositForm)) {
                submitFormToCGI(depositForm, '/cgi-bin/deposit.cgi', handleTransactionResponse);
            }
        });
    }

    const withdrawForm = document.getElementById('withdrawForm');
    if (withdrawForm) {
        withdrawForm.addEventListener('submit', (e) => {
            e.preventDefault();
            if (validateForm(withdrawForm)) {
                submitFormToCGI(withdrawForm, '/cgi-bin/withdraw.cgi', handleTransactionResponse);
            }
        });
    }

    const transferForm = document.getElementById('transferForm');
    if (transferForm) {
        transferForm.addEventListener('submit', (e) => {
            e.preventDefault();
            if (validateForm(transferForm)) {
                submitFormToCGI(transferForm, '/cgi-bin/transfer.cgi', handleTransactionResponse);
            }
        });
    }

    // Profile Form
    const profileForm = document.getElementById('profileForm');
    if (profileForm) {
        profileForm.addEventListener('submit', (e) => {
            e.preventDefault();
            const params = new URLSearchParams();
            params.append('action', 'update');
            params.append('name', document.getElementById('profileName').value);
            params.append('email', document.getElementById('profileEmail').value);
            params.append('phone', document.getElementById('profilePhone').value);
            params.append('address', document.getElementById('profileAddress').value);
            
            const oldPass = document.getElementById('old_password').value;
            const newPass = document.getElementById('new_password').value;
            if (oldPass) params.append('old_password', oldPass);
            if (newPass) params.append('new_password', newPass);

            fetch('/cgi-bin/profile.cgi', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                credentials: 'same-origin',
                body: params.toString()
            })
            .then(res => res.json())
            .then(data => {
                if (data.status === 'success') {
                    window.App.showToast(data.message, 'success');
                    // Clear password fields
                    document.getElementById('old_password').value = '';
                    document.getElementById('new_password').value = '';
                } else {
                    window.App.showToast(data.message, 'error');
                }
            })
            .catch(() => window.App.showToast('Server communication error', 'error'));
        });
    }
});

function handleTransactionResponse(data) {
    if (data.status === 'success') {
        window.App.showToast(data.message + ' New Balance: $' + data.new_balance.toFixed(2), 'success');
        setTimeout(() => { window.location.href = 'dashboard.html'; }, 2000);
    } else {
        window.App.showToast(data.message, 'error');
    }
}

// Utility to submit form data as URL-encoded string to CGI
function submitFormToCGI(formElement, url, callback) {
    const formData = new FormData(formElement);
    const urlEncodedData = new URLSearchParams(formData).toString();
    
    // Add any inputs that don't have name attributes by using their IDs (fallback for our html design)
    const inputs = formElement.querySelectorAll('input, select');
    const params = new URLSearchParams();
    inputs.forEach(input => {
        if (input.id && input.type !== 'submit' && input.type !== 'reset') {
            params.append(input.id, input.value);
        }
    });

    fetch(url, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
        },
        credentials: 'same-origin',
        body: params.toString()
    })
    .then(response => response.json())
    .then(data => callback(data))
    .catch(error => {
        console.error('Error:', error);
        window.App.showToast('Server communication error', 'error');
    });
}

function validateForm(form) {
    let isValid = true;
    const inputs = form.querySelectorAll('input[required], select[required]');

    inputs.forEach(input => {
        // Reset state
        clearError(input);

        if (!input.value.trim()) {
            showError(input, 'This field is required');
            isValid = false;
        } else if (input.type === 'email' && !validateEmail(input.value)) {
            showError(input, 'Please enter a valid email');
            isValid = false;
        } else if (input.type === 'number' && Number(input.value) <= 0 && !input.readOnly) {
            showError(input, 'Must be greater than 0');
            isValid = false;
        }
    });

    return isValid;
}

function showError(input, message) {
    input.classList.add('is-invalid');
    let feedback = input.parentElement.querySelector('.invalid-feedback');
    if (!feedback) {
        // Create it if it doesn't exist (e.g. if we didn't hardcode it in HTML)
        feedback = document.createElement('div');
        feedback.className = 'invalid-feedback';
        // Insert after the input group if it exists, else after input
        if (input.parentElement.classList.contains('input-icon')) {
             input.parentElement.after(feedback);
        } else {
            input.after(feedback);
        }
    }
    feedback.innerText = message;
    feedback.style.display = 'block';
}

function clearError(input) {
    input.classList.remove('is-invalid');
    let feedback = input.parentElement.querySelector('.invalid-feedback');
    if (!feedback && input.parentElement.classList.contains('input-icon')) {
        feedback = input.parentElement.nextElementSibling;
        if (feedback && feedback.classList.contains('invalid-feedback')) {
            feedback.style.display = 'none';
        }
    } else if (feedback) {
        feedback.style.display = 'none';
    }
}

function validateEmail(email) {
    const re = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    return re.test(email);
}
