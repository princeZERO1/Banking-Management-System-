/**
 * Main JavaScript File
 * Handles global UI interactions and dynamic data loading from C backend.
 */

// Global state/functions accessible everywhere
window.App = window.App || {};

document.addEventListener('DOMContentLoaded', () => {
    // Mobile Menu Toggle
    const mobileBtn = document.querySelector('.mobile-menu-btn');
    const navLinks = document.querySelector('.nav-links');
    
    if (mobileBtn && navLinks) {
        mobileBtn.addEventListener('click', () => {
            navLinks.style.display = navLinks.style.display === 'flex' ? 'none' : 'flex';
            navLinks.style.flexDirection = 'column';
            navLinks.style.position = 'absolute';
            navLinks.style.top = '100%';
            navLinks.style.left = '0';
            navLinks.style.width = '100%';
            navLinks.style.background = '#fff';
            navLinks.style.padding = '20px';
            navLinks.style.boxShadow = '0 5px 10px rgba(0,0,0,0.1)';
        });
    }

    // Password Visibility Toggle
    const togglePasswordIcons = document.querySelectorAll('.toggle-password');
    togglePasswordIcons.forEach(icon => {
        icon.addEventListener('click', function() {
            const input = this.previousElementSibling;
            if (input && input.type === 'password') {
                input.type = 'text';
                this.classList.remove('fa-eye');
                this.classList.add('fa-eye-slash');
            } else if (input && input.type === 'text') {
                input.type = 'password';
                this.classList.remove('fa-eye-slash');
                this.classList.add('fa-eye');
            }
        });
    });

    // ========== DYNAMIC DATA LOADING ==========
    // Only run on dashboard pages (pages with sidebar/dashboard-container)
    const isDashboardPage = document.querySelector('.dashboard-container') !== null;
    if (!isDashboardPage) return;

    // Fetch user data from the dashboard CGI endpoint
    fetch('/cgi-bin/dashboard.cgi', {
        method: 'POST',
        credentials: 'same-origin'
    })
    .then(res => res.json())
    .then(data => {
        if (data.status === 'success' && data.customer) {
            const c = data.customer;
            const firstName = c.name.split(' ')[0];
            const fmtBal = c.balance.toLocaleString('en-IN', {minimumFractionDigits: 2, maximumFractionDigits: 2});

            // --- 1. Update ALL avatar images (header + profile card) ---
            document.querySelectorAll('.user-profile img, .glass-card img').forEach(img => {
                if (img.src.includes('ui-avatars.com')) {
                    const size = img.src.includes('size=150') ? '150' : '40';
                    img.src = `https://ui-avatars.com/api/?name=${encodeURIComponent(c.name)}&background=0D8ABC&color=fff&size=${size}`;
                }
            });

            // --- 2. Global text node replacement ---
            const replacements = {
                'John Doe': c.name,
                'Welcome back, John!': `Welcome back, ${firstName}!`,
                '1000 1234 5678': c.account_number,
                'Acc No: 1000 1234 5678': 'Acc No: ' + c.account_number,
                'Account Number: 1000 1234 5678': 'Account Number: ' + c.account_number,
                '$ 12,450.00': '\u20b9 ' + fmtBal,
                '$12,450.00': '\u20b9' + fmtBal,
                '\u20b9 12,450.00': '\u20b9 ' + fmtBal,
                '\u20b912,450.00': '\u20b9' + fmtBal,
                'CUST-10932': 'CUST-' + c.account_number.slice(-5),
                'XXXX XXXX 4589': c.aadhaar ? 'XXXX XXXX ' + c.aadhaar.slice(-4) : 'XXXX XXXX ****',
                'ABCDE1***F': c.pan ? c.pan.substring(0,5) + '***' + c.pan.slice(-1) : '**********',
                'john.doe@example.com': c.email || 'N/A',
                '+1 (555) 123-4567': c.phone || 'N/A',
                '123 Finance Street, NY 10001': c.address || 'N/A',
            };

            const walk = document.createTreeWalker(document.body, NodeFilter.SHOW_TEXT, null, false);
            let node;
            while (node = walk.nextNode()) {
                let val = node.nodeValue;
                let changed = false;
                for (const [find, replace] of Object.entries(replacements)) {
                    if (val.includes(find)) {
                        val = val.split(find).join(replace);
                        changed = true;
                    }
                }
                if (changed) node.nodeValue = val;
            }

            // --- 3. Update input field values ---
            document.querySelectorAll('input').forEach(input => {
                if (input.value === 'John Doe') input.value = c.name;
                if (input.value === 'john.doe@example.com') input.value = c.email || '';
                if (input.value === '+1 (555) 123-4567') input.value = c.phone || '';
                if (input.value === '123 Finance Street, NY 10001') input.value = c.address || '';
                if (input.id === 'accNumberDisplay') input.value = c.account_number;
                if (input.id === 'currentBalanceDisplay') input.value = '₹ ' + fmtBal;
            });

            // --- 4. Render dynamic transaction table ---
            const tbody = document.querySelector('.data-table tbody');
            if (tbody && data.transactions) {
                tbody.innerHTML = '';
                if (data.transactions.length === 0) {
                    const colCount = document.querySelectorAll('.data-table thead th').length || 5;
                    tbody.innerHTML = `<tr><td colspan="${colCount}" style="text-align:center; padding:30px; color:#999;">No transactions yet. Make a deposit to get started!</td></tr>`;
                } else {
                    const txns = [...data.transactions].reverse();
                    const thFirst = document.querySelector('.data-table thead th:first-child');
                    const hasIdCol = thFirst && thFirst.textContent.trim().includes('ID');

                    txns.forEach(txn => {
                        const isDeposit = txn.type.toLowerCase() === 'deposit';
                        const isTransfer = txn.type.toLowerCase() === 'transfer';
                        const badgeClass = isDeposit ? 'badge-success' : (isTransfer ? 'badge-warning' : 'badge-danger');
                        const sign = isDeposit ? '+' : '-';
                        const amountStr = sign + '₹' + txn.amount.toLocaleString('en-IN', {minimumFractionDigits: 2});
                        const balStr = txn.balance != null ? '₹' + txn.balance.toLocaleString('en-IN', {minimumFractionDigits: 2}) : '-';

                        const tr = document.createElement('tr');
                        if (hasIdCol) {
                            tr.innerHTML = `
                                <td>${txn.id || 'N/A'}</td>
                                <td>${txn.date}</td>
                                <td><span class="badge ${badgeClass}">${txn.type}</span></td>
                                <td>${txn.description}</td>
                                <td>${amountStr}</td>
                                <td>${balStr}</td>
                                <td>Completed</td>
                            `;
                        } else {
                            tr.innerHTML = `
                                <td>${txn.date}</td>
                                <td><span class="badge ${badgeClass}">${txn.type}</span></td>
                                <td>${txn.description}</td>
                                <td>${amountStr}</td>
                                <td>Completed</td>
                            `;
                        }
                        tbody.appendChild(tr);
                    });
                }
            }

            // --- 5. Update pagination text ---
            const paginationText = document.querySelector('.dashboard-container p[style*="Showing"]');
            if (paginationText && data.transactions) {
                const count = data.transactions.length;
                paginationText.textContent = `Showing 1 to ${count} of ${count} entries`;
            }

            // --- 6. Update "Last Updated" on balance page ---
            const lastUpdated = document.getElementById('lastUpdated');
            if (lastUpdated) {
                const now = new Date();
                lastUpdated.textContent = 'Today at ' + now.toLocaleTimeString('en-US', {hour: '2-digit', minute: '2-digit'});
            }

            // Store data globally for use by other scripts (e.g. refreshBalance)
            window.App.userData = data;

        } else {
            // Not logged in — redirect to login
            window.location.href = 'login.html';
        }
    })
    .catch(err => {
        console.error('Error fetching dashboard data:', err);
        // If CGI is unreachable (e.g. not running through XAMPP), don't redirect
    });
});

// ========== GLOBAL REFRESH BALANCE FUNCTION ==========
window.refreshBalance = function() {
    const icon = document.getElementById('syncIcon');
    if (icon) icon.classList.add('fa-spin');

    fetch('/cgi-bin/balance.cgi', {
        method: 'POST',
        credentials: 'same-origin'
    })
    .then(res => res.json())
    .then(data => {
        if (icon) icon.classList.remove('fa-spin');
        if (data.status === 'success') {
            const fmtBal = data.balance.toLocaleString('en-IN', {minimumFractionDigits: 2, maximumFractionDigits: 2});
            // Update the big balance display
            const balDisplay = document.querySelector('h1[style*="success"]');
            if (balDisplay) balDisplay.textContent = '₹ ' + fmtBal;
            document.getElementById('lastUpdated').textContent = 'Just now';
            window.App.showToast('Balance Refreshed!', 'success');
        } else {
            window.App.showToast(data.message || 'Error refreshing', 'error');
        }
    })
    .catch(() => {
        if (icon) icon.classList.remove('fa-spin');
        window.App.showToast('Server communication error', 'error');
    });
};

// ========== PDF EXPORT FOR MINI STATEMENT ==========
window.exportStatementPDF = function() {
    const userData = window.App.userData;
    const customer = userData ? userData.customer : null;
    const transactions = userData ? userData.transactions : [];

    // Build a printable HTML page content
    const now = new Date();
    const dateStr = now.toLocaleDateString('en-IN', {day:'2-digit', month:'long', year:'numeric'});
    const timeStr = now.toLocaleTimeString('en-IN', {hour:'2-digit', minute:'2-digit'});

    let txnRows = '';
    if (transactions && transactions.length > 0) {
        [...transactions].reverse().forEach((txn, i) => {
            const isDeposit = txn.type.toLowerCase() === 'deposit';
            const sign = isDeposit ? '+' : '-';
            const amtFormatted = sign + '₹' + txn.amount.toLocaleString('en-IN', {minimumFractionDigits: 2});
            const balFormatted = txn.balance != null ? '₹' + txn.balance.toLocaleString('en-IN', {minimumFractionDigits: 2}) : '-';
            const rowBg = i % 2 === 0 ? '#f9f9f9' : '#fff';
            txnRows += `
                <tr style="background:${rowBg}">
                    <td style="padding:10px;border-bottom:1px solid #eee;">${txn.id || 'N/A'}</td>
                    <td style="padding:10px;border-bottom:1px solid #eee;">${txn.date}</td>
                    <td style="padding:10px;border-bottom:1px solid #eee;">${txn.type}</td>
                    <td style="padding:10px;border-bottom:1px solid #eee;">${txn.description}</td>
                    <td style="padding:10px;border-bottom:1px solid #eee;font-weight:bold;color:${isDeposit?'#2e7d32':'#c62828'}">${amtFormatted}</td>
                    <td style="padding:10px;border-bottom:1px solid #eee;">${balFormatted}</td>
                </tr>`;
        });
    } else {
        txnRows = '<tr><td colspan="6" style="padding:20px;text-align:center;color:#999;">No transactions found</td></tr>';
    }

    const accountNumber = customer ? customer.account_number : 'N/A';
    const customerName = customer ? customer.name : 'N/A';
    const accountType = customer ? customer.account_type : 'N/A';
    const balance = customer ? '₹' + customer.balance.toLocaleString('en-IN', {minimumFractionDigits:2}) : 'N/A';

    const printContent = `
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="UTF-8">
            <title>Account Statement - NexusBank</title>
            <style>
                body { font-family: Arial, sans-serif; color: #333; margin: 0; padding: 20px; }
                .header { background: linear-gradient(135deg, #1a3a5c, #2980b9); color: #fff; padding: 25px 30px; border-radius: 8px; margin-bottom: 25px; }
                .header h1 { margin: 0 0 5px; font-size: 24px; }
                .header p  { margin: 0; font-size: 13px; opacity: 0.85; }
                .info-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; margin-bottom: 25px; }
                .info-box { background: #f4f7fb; padding: 15px; border-radius: 6px; border-left: 4px solid #2980b9; }
                .info-box label { font-size: 11px; color: #888; text-transform: uppercase; letter-spacing: 1px; }
                .info-box p { margin: 4px 0 0; font-size: 15px; font-weight: bold; color: #1a3a5c; }
                table { width: 100%; border-collapse: collapse; }
                thead { background: #1a3a5c; color: #fff; }
                thead th { padding: 12px 10px; text-align: left; font-size: 13px; }
                tbody tr:hover { background: #edf3fb !important; }
                .footer { margin-top: 30px; text-align: center; font-size: 12px; color: #999; border-top: 1px solid #eee; padding-top: 15px; }
                @media print { body { padding: 10px; } }
            </style>
        </head>
        <body>
            <div class="header">
                <h1>⌂ NexusBank</h1>
                <p>Official Account Statement &mdash; Generated on ${dateStr} at ${timeStr}</p>
            </div>
            <div class="info-grid">
                <div class="info-box"><label>Account Holder</label><p>${customerName}</p></div>
                <div class="info-box"><label>Account Number</label><p>${accountNumber}</p></div>
                <div class="info-box"><label>Account Type</label><p>${accountType}</p></div>
                <div class="info-box"><label>Current Balance</label><p>${balance}</p></div>
            </div>
            <table>
                <thead>
                    <tr>
                        <th>Txn ID</th><th>Date</th><th>Type</th><th>Description</th><th>Amount</th><th>Balance</th>
                    </tr>
                </thead>
                <tbody>${txnRows}</tbody>
            </table>
            <div class="footer">This is a computer-generated statement. No signature required. &copy; NexusBank ${now.getFullYear()}</div>
        </body>
        </html>`;

    const win = window.open('', '_blank', 'width=900,height=700');
    win.document.write(printContent);
    win.document.close();
    win.focus();
    setTimeout(() => { win.print(); }, 500);
};

/**
 * Show a toast notification
 * @param {string} message - The message to display
 * @param {string} type - 'success' or 'error'
 */
window.App.showToast = function(message, type = 'success') {
    let container = document.querySelector('.toast-container');
    
    // Create container if it doesn't exist
    if (!container) {
        container = document.createElement('div');
        container.className = 'toast-container';
        document.body.appendChild(container);
    }

    const toast = document.createElement('div');
    toast.className = `toast ${type}`;
    
    const icon = type === 'success' ? 'fa-check-circle' : 'fa-exclamation-circle';
    
    toast.innerHTML = `
        <i class="fas ${icon}"></i>
        <div class="toast-message">${message}</div>
    `;

    container.appendChild(toast);

    // Trigger animation
    setTimeout(() => {
        toast.classList.add('show');
    }, 10);

    // Remove after 3 seconds
    setTimeout(() => {
        toast.classList.remove('show');
        setTimeout(() => {
            toast.remove();
        }, 300);
    }, 3000);
};
