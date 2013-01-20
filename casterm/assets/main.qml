import bb.cascades 1.0

Page {
    Container {
        objectName: "terminal"
        ListView {
            objectName: "terminalLines"
            listItemComponents: [
                ListItemComponent {
                    StandardListItem {
                        title: ListItemData
                    }
                }
            ]
        }
    }
}
