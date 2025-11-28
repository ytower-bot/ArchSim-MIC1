package main

import (
	"github.com/charmbracelet/lipgloss"
)

var (
	titleStyle = lipgloss.NewStyle().
			Bold(true).
			Foreground(lipgloss.Color("#FFFFFF")).
			Background(lipgloss.Color("#7D56F4")).
			Padding(0, 1)

	borderStyle = lipgloss.NewStyle().
			BorderStyle(lipgloss.RoundedBorder()).
			BorderForeground(lipgloss.Color("#7D56F4"))

	helpStyle = lipgloss.NewStyle().
			BorderStyle(lipgloss.RoundedBorder()).
			BorderForeground(lipgloss.Color("#FFFF00")).
			Padding(1, 2).
			Width(60)

	statusStyle = lipgloss.NewStyle().
			Foreground(lipgloss.Color("#FFFFFF")).
			Background(lipgloss.Color("#7D56F4")).
			Padding(0, 1)

	panelStyle = lipgloss.NewStyle().
			BorderStyle(lipgloss.RoundedBorder()).
			BorderForeground(lipgloss.Color("#7D56F4")).
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

	// Title bar
	title := titleStyle.Width(m.width - 2).Render("MIC-1 SIMULATOR")

	// Status bar
	status := statusStyle.Width(m.width - 2).Render(m.getStatusLine())

	// Split view
	leftWidth := m.getLeftPanelWidth() - 6
	rightWidth := m.getRightPanelWidth() - 6
	contentHeight := m.height - 6

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
		Foreground(lipgloss.Color("#666666")).
		Render("[s] Step  [r] Run  [x] Reset  [l] Load  [h] Help  [q] Quit")

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
