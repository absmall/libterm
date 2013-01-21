import bb.cascades 1.0

Page {
    Container {
        objectName: "terminal"
        ListView {
            objectName: "terminalLines"
            focusPolicy: FocusPolicy.KeyAndTouch
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
