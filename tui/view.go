package main

import (
	"github.com/charmbracelet/lipgloss"
)

var (
	titleStyle = lipgloss.NewStyle().
			Bold(true).
			Foreground(lipgloss.Color("#FFFFFF")).
			Background(lipgloss.Color("#0066CC")).
			Padding(0, 1)

	borderStyle = lipgloss.NewStyle().
			BorderStyle(lipgloss.RoundedBorder()).
			BorderForeground(lipgloss.Color("#666666"))

	helpStyle = lipgloss.NewStyle().
			BorderStyle(lipgloss.RoundedBorder()).
			BorderForeground(lipgloss.Color("#0066CC")).
			Padding(1, 2).
			Width(60)

	statusStyle = lipgloss.NewStyle().
			Foreground(lipgloss.Color("#FFFFFF")).
			Background(lipgloss.Color("#333333")).
			Padding(0, 1)

	panelStyle = lipgloss.NewStyle().
			BorderStyle(lipgloss.RoundedBorder()).
			BorderForeground(lipgloss.Color("#666666")).
			Padding(1, 2)
)

func (m model) View() string {
	if m.width == 0 {
		return "Initializing..."
	}

	// Show help overlay
	if m.showHelp {
		return m.renderHelpView()
	}

	// Show microcode viewer overlay
	if m.showMicrocode {
		return m.renderMicrocodeView()
	}

	// Show memory viewer overlay
	if m.showMemoryView {
		return m.renderMemoryView()
	}

	// Show file picker overlay
	if m.showFilePicker {
		return m.renderFilePickerView()
	}

	// Show results viewer overlay
	if m.showResultsView {
		return m.renderResultsView()
	}

	// Show cache viewer overlay
	if m.showCacheView {
		return m.renderCacheView()
	}

	// Title bar
	title := titleStyle.Width(m.width - 2).Render("MIC-1 SIMULATOR")

	// Status bar
	status := statusStyle.Width(m.width - 2).Render(m.getStatusLine())

	// Split view with bounds checking
	leftWidth := m.width/2 - 4
	if leftWidth < 20 {
		leftWidth = 20
	}

	rightWidth := m.width/2 - 4
	if rightWidth < 20 {
		rightWidth = 20
	}

	contentHeight := m.height - 8
	if contentHeight < 10 {
		contentHeight = 10
	}

	leftPanel := panelStyle.
		Width(leftWidth).
		Height(contentHeight).
		Render(m.renderSourceCode())

	rightPanel := panelStyle.
		Width(rightWidth).
		Height(contentHeight).
		Render(m.renderState())

	content := lipgloss.JoinHorizontal(lipgloss.Top, leftPanel, rightPanel)

	// Controls hint
	controls := lipgloss.NewStyle().
		Foreground(lipgloss.Color("#888888")).
		Render("[s] Step [r] Run [x] Reset [l] Load | [t] Results [c] Cache [d] Memory | [h] Help [q] Quit")

	return lipgloss.JoinVertical(
		lipgloss.Left,
		title,
		content,
		controls,
		status,
	)
}

func (m model) renderHelpView() string {
	help := helpStyle.Render(m.renderHelp())

	return lipgloss.Place(
		m.width,
		m.height,
		lipgloss.Center,
		lipgloss.Center,
		help,
		lipgloss.WithWhitespaceChars(" "),
		lipgloss.WithWhitespaceForeground(lipgloss.Color("#1a1a1a")),
	)
}

func (m model) renderMicrocodeView() string {
	// Bounds checking for overlay dimensions
	overlayWidth := m.width - 10
	if overlayWidth < 40 {
		overlayWidth = 40
	}
	if overlayWidth > m.width {
		overlayWidth = m.width - 2
	}

	overlayHeight := m.height - 10
	if overlayHeight < 10 {
		overlayHeight = 10
	}
	if overlayHeight > m.height {
		overlayHeight = m.height - 2
	}

	microcodeStyle := lipgloss.NewStyle().
		BorderStyle(lipgloss.RoundedBorder()).
		BorderForeground(lipgloss.Color("#00AA00")).
		Padding(1, 2).
		Width(overlayWidth).
		Height(overlayHeight)

	microcode := microcodeStyle.Render(m.renderMicrocode())

	return lipgloss.Place(
		m.width,
		m.height,
		lipgloss.Center,
		lipgloss.Center,
		microcode,
		lipgloss.WithWhitespaceChars(" "),
		lipgloss.WithWhitespaceForeground(lipgloss.Color("#1a1a1a")),
	)
}

func (m model) renderFilePickerView() string {
	// Bounds checking for overlay dimensions
	overlayWidth := m.width - 20
	if overlayWidth < 50 {
		overlayWidth = 50
	}
	if overlayWidth > m.width {
		overlayWidth = m.width - 2
	}

	overlayHeight := m.height - 10
	if overlayHeight < 20 {
		overlayHeight = 20
	}
	if overlayHeight > m.height {
		overlayHeight = m.height - 2
	}

	pickerStyle := lipgloss.NewStyle().
		BorderStyle(lipgloss.RoundedBorder()).
		BorderForeground(lipgloss.Color("#FFA500")).
		Padding(1, 2).
		Width(overlayWidth).
		Height(overlayHeight)

	picker := pickerStyle.Render(m.renderFilePicker())

	return lipgloss.Place(
		m.width,
		m.height,
		lipgloss.Center,
		lipgloss.Center,
		picker,
		lipgloss.WithWhitespaceChars(" "),
		lipgloss.WithWhitespaceForeground(lipgloss.Color("#1a1a1a")),
	)
}

func (m model) renderResultsView() string {
	overlayWidth := m.width - 20
	if overlayWidth < 50 {
		overlayWidth = 50
	}
	if overlayWidth > m.width {
		overlayWidth = m.width - 2
	}

	overlayHeight := m.height - 10
	if overlayHeight < 20 {
		overlayHeight = 20
	}
	if overlayHeight > m.height {
		overlayHeight = m.height - 2
	}

	resultsStyle := lipgloss.NewStyle().
		BorderStyle(lipgloss.RoundedBorder()).
		BorderForeground(lipgloss.Color("#00AA00")).
		Padding(1, 2).
		Width(overlayWidth).
		Height(overlayHeight)

	results := resultsStyle.Render(m.renderResultsDetailed())

	return lipgloss.Place(
		m.width,
		m.height,
		lipgloss.Center,
		lipgloss.Center,
		results,
		lipgloss.WithWhitespaceChars(" "),
		lipgloss.WithWhitespaceForeground(lipgloss.Color("#1a1a1a")),
	)
}

func (m model) renderCacheView() string {
	overlayWidth := m.width - 20
	if overlayWidth < 50 {
		overlayWidth = 50
	}
	if overlayWidth > m.width {
		overlayWidth = m.width - 2
	}

	overlayHeight := m.height - 10
	if overlayHeight < 20 {
		overlayHeight = 20
	}
	if overlayHeight > m.height {
		overlayHeight = m.height - 2
	}

	cacheStyle := lipgloss.NewStyle().
		BorderStyle(lipgloss.RoundedBorder()).
		BorderForeground(lipgloss.Color("#00AA00")).
		Padding(1, 2).
		Width(overlayWidth).
		Height(overlayHeight)

	cache := cacheStyle.Render(m.renderCacheDetailed())

	return lipgloss.Place(
		m.width,
		m.height,
		lipgloss.Center,
		lipgloss.Center,
		cache,
		lipgloss.WithWhitespaceChars(" "),
		lipgloss.WithWhitespaceForeground(lipgloss.Color("#1a1a1a")),
	)
}
